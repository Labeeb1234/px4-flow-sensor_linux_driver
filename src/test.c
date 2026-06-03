#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include "common/mavlink.h"  // from c_library_v2

// open serial port
int open_serial(const char* port, int baud) {
    int fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);
    struct termios tty = {0};
    tcgetattr(fd, &tty);
    cfsetispeed(&tty, baud);
    cfsetospeed(&tty, baud);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tcsetattr(fd, TCSANOW, &tty);
    return fd;
}


void read_message_loop(int fd){
    mavlink_message_t msg;
    mavlink_status_t status;
    int message_received = 0;
    uint8_t c;

    while(read(fd, &c, 1) > 0){
        message_received = mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status);
        if(message_received){
            printf("Received message from serial with ID #%d (sys:%d|comp:%d):\n", msg.msgid, msg.sysid, msg.compid);
            if(msg.msgid == MAVLINK_MSG_ID_OPTICAL_FLOW_RAD){
                mavlink_optical_flow_rad_t flow;
                mavlink_msg_optical_flow_rad_decode(
                    &msg,
                    &flow
                );
                printf("OPTICAL_FLOW_RAD\n");
                printf("integration_time_us : %u\n", flow.integration_time_us);
                printf("integrated_x        : %f\n", flow.integrated_x);
                printf("integrated_y        : %f\n", flow.integrated_y);
                printf("distance            : %f\n", flow.distance);
                printf("quality             : %u\n", flow.quality);


            }else if(msg.msgid == MAVLINK_MSG_ID_DISTANCE_SENSOR){
                mavlink_distance_sensor_t distance;
                mavlink_msg_distance_sensor_decode(
                    &msg,
                    &distance
                );
                printf("DISTANCE_SENSOR\n");
                printf("current_distance : %u cm\n", distance.current_distance);
                printf("min_distance     : %u cm\n", distance.min_distance);
                printf("max_distance     : %u cm\n", distance.max_distance);
                printf("orientation      : %u\n", distance.orientation);
            }else if(msg.msgid == MAVLINK_MSG_ID_ENCAPSULATED_DATA){
                mavlink_encapsulated_data_t encap;
                mavlink_msg_encapsulated_data_encode(
                    msg.sysid,
                    msg.compid,
                    &msg,
                    &encap
                );

                printf("seqnr : %u\n", encap.seqnr);
                printf("first 32 bytes:\n");
                for(int i = 0; i < 32; i++){
                    printf("%02X ", encap.data[i]);
                }
                printf("\n");

            }else{
                fprintf(stderr, "Unknown msg id: %d\n", msg.msgid);
            } 
        }
        
    }
}



int main(int argc, char** argv){  
    const char* port_id = "/dev/ttyACM0";
    int baudrate = 115200;    
        
    int fd = open_serial(port_id, baudrate);
    printf("port open status: %d\n", fd);

    if(fd > 0){
        read_message_loop(fd);
    }

    return 0;
}