#include <iostream>
#include <cstdio>
#include <cstdint>
#include <chrono>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "linux/i2c-dev.h"

extern "C"{
        #include "i2c/smbus.h"
}

#include "optical_flow/px4_flow.hpp"


#define PX4_REG_FRAME 0x00
/*
        Test func to connect to the i2c device (px4 flow sensor)
*/
int read_port(const char* i2c_device="/dev/i2c-1"){
        int fd = open(i2c_device, O_RDWR);

        if(fd < 0){
                fprintf(stderr, "Failed to open I2C bus\n");
                return -1;
        }
        if(ioctl(fd, I2C_SLAVE, PX4FLOW_ADDRESS) < 0){
                fprintf(stderr, "Failed to connect to PX4FLOW\n");
                close(fd);
                return -1;
        }
        // printf("Connected to PX4FLOW!\n"); --> should not be here like this bad logic based on the port availablity on the system
        return fd;
}
uint8_t read8(const uint8_t* buffer, int& idx){
        return buffer[idx++];
}
uint16_t read16(const uint8_t* buffer, int& idx){
        uint16_t value =
                (uint16_t)buffer[idx] |
                ((uint16_t)buffer[idx + 1] << 8);
        idx+=2;
        return value; 
}
uint32_t read32(const uint8_t* buffer, int& idx){
        uint32_t low = read16(buffer, idx);
        uint32_t high = read16(buffer, idx);
        return low | (high << 16);
}




bool update_frame(int& fd, i2c_frame& frame){
        if(fd < 0){
                fprintf(stderr, "px4 flow not connected!");
                return false;
        }
        uint8_t reg = PX4_REG_FRAME;

        // write to register to request data
        if(write(fd, &reg, 1) != 1){
                fprintf(stderr, "Failed to select frame register!\n");
                return false;
        }

        // usleep(1000);
        uint8_t buffer[22];
        //  read data
        // auto start = std::chrono::steady_clock::now();
        ssize_t bytes_read = read(fd, buffer, 22);
        // auto end = std::chrono::steady_clock::now();
        // int bytes_read = i2c_smbus_read_i2c_block_data(
        //      fd,
        //      PX4_REG_FRAME,
        //      22,
        //      buffer
        // );
        if(bytes_read < 0){
                fprintf(stderr, "I2C read failed");
                return false;
        }
        if(bytes_read != 22){
                fprintf(stderr, "[PX4Flow] Expected 22 bytes, got %ld\n", bytes_read);
                return false;
        }
        // parsing data buffer
        int idx=0; // buffer data byte index
        frame.frame_count = read16(buffer, idx);
        frame.pixel_flow_x_sum = read16(buffer, idx);
        frame.pixel_flow_y_sum = read16(buffer, idx);
        frame.flow_comp_m_x = read16(buffer, idx);
        frame.flow_comp_m_y = read16(buffer, idx);
        frame.qual = read16(buffer, idx);
        frame.gyro_x_rate = read16(buffer, idx);
        frame.gyro_y_rate = read16(buffer, idx);
        frame.gyro_z_rate = read16(buffer, idx);
        frame.gyro_range = read8(buffer, idx);
        frame.sonar_timestamp = read8(buffer, idx);
        frame.ground_distance = read16(buffer, idx);
        // instead of this wait for 22 bytes
        // is this required while using read() of the i2c-dev header
        // while(bytes_read < 22){
        //      if(std::chrono::duration_cast<std::chrono::milliseconds>(now-start).count() > PX4FLO>
        //              fprintf(stderr, "[PX4Flow] : Timeout reading PX4_Flow.");
        //      }
        // }
        return true;
}




void print_frame(i2c_frame& frame){
        std::cout << "\n===== SIMPLE FRAME =====\n";

        std::cout
            << "frame_count      : "
            << frame.frame_count
            << "\n";

        std::cout
            << "pixel_flow_x_sum : "
            << frame.pixel_flow_x_sum
            << "\n";

        std::cout
            << "pixel_flow_y_sum : "
            << frame.pixel_flow_y_sum
            << "\n";

        std::cout
            << "flow_comp_m_x    : "
            << frame.flow_comp_m_x
            << "\n";

        std::cout
            << "flow_comp_m_y    : "
            << frame.flow_comp_m_y
            << "\n";

        std::cout
            << "quality          : "
            << frame.qual
            << "\n";

        std::cout
            << "gyro_x_rate      : "
            << frame.gyro_x_rate
            << "\n";

        std::cout
            << "gyro_y_rate      : "
            << frame.gyro_y_rate
            << "\n";

        std::cout
            << "gyro_z_rate      : "
            << frame.gyro_z_rate
            << "\n";
       
		std::cout
            << "ground_distance  : "
            << frame.ground_distance
            << "\n";
}


int main(int argc, char** argv){
        // simple i2c device detection and open and receive data
        i2c_frame frame;
        int fd = read_port();
        while(true){
                if(fd < 0){
                        break;
                }
                if(update_frame(fd, frame)){
                        // if(frame.qual > 100){
                                print_frame(frame);
                        // }else{
                        //      printf(".\n");
                        //}
                }
                usleep(100000);
        }
        close(fd);
        return 0;
}





