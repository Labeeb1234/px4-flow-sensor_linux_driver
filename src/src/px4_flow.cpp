#include "px4_flow.hpp"


PX4Flow::PX4Flow(){}
PX4Flow::~PX4Flow(){}

bool PX4Flow::update(int& fd){
    if(fd < 0){
		fprintf(stderr, "px4 flow not connected!");
		return false;
    }

    uint8_t reg = PX4_REG_FRAME;
    // write and request frame reg from flow sensor
    if(write(fd, &reg, 1) != 1){
        fprintf(stderr, "Failed to select frame register!\n");
        return false;
    }

    // usleep(100); // add delay here to complete requesting of frames (double tapping) ???
    // read frame data
    uint8_t rx_buffer[RX_FRAME_BUFFER_LEN];
    auto start = std::chrono::steady_clock::now(); // using steady clock to prevent issues with time jumps
    ssize_t bytes_read = read(fd, rx_buffer, RX_FRAME_BUFFER_LEN);
    auto end = std::chrono::steady_clock::now();
    auto elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();

    if(elapsed_time_ms > PX4FLOW_TIMEOUT){
        fprintf(stderr, "[PX4Flow] : Timeout reading PX4_Flow.");
        return false;
    }
    if(bytes_read < 0){
		fprintf(stderr, "I2C read failed");
		return false;
    }
    if(bytes_read != RX_FRAME_BUFFER_LEN){
		fprintf(stderr, "[PX4Flow] Expected 22 bytes, got %ld\n", bytes_read);
		return false;
    }

    int rx_buffer_idx = 0;
    frame.frame_count = read16(rx_buffer, rx_buffer_idx);
    frame.pixel_flow_x_sum = read16(rx_buffer, rx_buffer_idx);
    frame.pixel_flow_y_sum = read16(rx_buffer, rx_buffer_idx);
    frame.flow_comp_m_x = read16(rx_buffer, rx_buffer_idx);
    frame.flow_comp_m_y = read16(rx_buffer, rx_buffer_idx);
    frame.qual = read16(rx_buffer, rx_buffer_idx);
    frame.gyro_x_rate = read16(rx_buffer, rx_buffer_idx);
    frame.gyro_y_rate = read16(rx_buffer, rx_buffer_idx);
    frame.gyro_z_rate = read16(rx_buffer, rx_buffer_idx);
    frame.gyro_range = read8(rx_buffer, rx_buffer_idx);
    frame.sonar_timestamp = read8(rx_buffer, rx_buffer_idx);
    frame.ground_distance = read16(rx_buffer, rx_buffer_idx);

    // drain overflow ??
    // if(rx_buffer_idx >= RX_FRAME_BUFFER_LEN){
    //     #if PX4FLOW_DEBUG == true
    //     {
    //         fprintf(stderr, "Too many bytes available");
    //     }
    //     #endif
    //     while(rx_buffer_idx >= RX_FRAME_BUFFER_LEN){read8();}
    // }
    return true;
}



void PX4Flow::display_frame(){
    printf("\n===== DISPLAY FRAME =====\n");
    printf("frame_count      : %u\n", frame.frame_count);
    printf("pixel_flow_x_sum : %d\n", frame.pixel_flow_x_sum);
    printf("pixel_flow_y_sum : %d\n", frame.pixel_flow_y_sum);
    printf("flow_comp_m_x    : %d\n", frame.flow_comp_m_x);
    printf("flow_comp_m_y    : %d\n", frame.flow_comp_m_y);
    printf("quality          : %u\n", frame.qual);
    printf("gyro_x_rate      : %d\n", frame.gyro_x_rate);
    printf("gyro_y_rate      : %d\n", frame.gyro_y_rate);
    printf("gyro_z_rate      : %d\n", frame.gyro_z_rate);
    printf("gyro range       : %u\n", frame.gyro_range);
    printf("sonar timestamp  : %u\n", frame.sonar_timestamp);
    printf("ground_distance  : %u\n", frame.ground_distance);
}
void PX4Flow::display_iframe(){
    printf("\n===== DISPLAY INTEGRATED FRAME =====\n");
    printf("frame count since last readout      : %u\n", iframe.frame_count_since_last_readout);
    printf("pixel_flow_x_integral               : %d\n", iframe.pixel_flow_x_integral);
    printf("pixel_flow_y_integral               : %d\n", iframe.pixel_flow_y_integral);
    printf("gyro_x_rate_integral                : %d\n", iframe.gyro_x_rate_integral);
    printf("gyro_y_rate integral                : %d\n", iframe.gyro_y_rate_integral);
    printf("gyro_z_rate integral                : %d\n", iframe.gyro_z_rate_integral);
    printf("integration timespan                : %u\n", iframe.integration_timespan);
    printf("sonar timestamp                     : %u\n", iframe.sonar_timestamp);
    printf("ground distance                     : %d\n", iframe.ground_distance);
    printf("gyro temperature                    : %d\n", iframe.gyro_temperature);
    printf("quality                             : %u\n", iframe.quality);
} 


// flow frame field value extraction 
uint16_t PX4Flow::frame_count(){
    return frame.frame_count;
}
int16_t PX4Flow::pixel_flow_x_sum(){
    return frame.pixel_flow_x_sum;
}
int16_t PX4Flow::pixel_flow_y_sum(){
    return frame.pixel_flow_y_sum;
}
int16_t PX4Flow::flow_comp_m_x(){
    return frame.flow_comp_m_x;
}
int16_t PX4Flow::flow_comp_m_y(){
    return frame.flow_comp_m_y;
}
int16_t PX4Flow::gyro_x_rate(){
    return frame.gyro_x_rate;
}
int16_t PX4Flow::gyro_y_rate(){
    return frame.gyro_y_rate;
}
int16_t PX4Flow::gyro_z_rate(){
    return frame.gyro_z_rate;
}
int16_t PX4Flow::qual(){
    return frame.qual;
}
uint8_t PX4Flow::sonar_timestamp() {
  return frame.sonar_timestamp;
}

int16_t PX4Flow::ground_distance() {
  return frame.ground_distance;
}

// flow integrated w/ gyro data to prevent rotational variations to accumulate with the flow translational data
uint16_t PX4Flow::frame_count_since_last_readout() {
  return iframe.frame_count_since_last_readout;
}
int16_t PX4Flow::pixel_flow_x_integral() {
  return iframe.pixel_flow_x_integral;
}
int16_t PX4Flow::pixel_flow_y_integral() {
  return iframe.pixel_flow_y_integral;
}
int16_t PX4Flow::gyro_x_rate_integral() {
  return iframe.gyro_x_rate_integral;
}
int16_t PX4Flow::gyro_y_rate_integral() {
  return iframe.gyro_y_rate_integral;
}
int16_t PX4Flow::gyro_z_rate_integral() {
  return iframe.gyro_z_rate_integral;
}
uint32_t PX4Flow::integration_timespan() {
  return iframe.integration_timespan;
}
uint32_t PX4Flow::sonar_timestamp_integral() {
  return iframe.sonar_timestamp;
}     
int16_t PX4Flow::ground_distance_integral() {
  return iframe.ground_distance;
}
int16_t PX4Flow::gyro_temperature() {
  return iframe.gyro_temperature;
}
uint8_t PX4Flow::quality_integral() {
  return iframe.quality;
}


// data parsing helpers
uint8_t PX4Flow::read8(const uint8_t* buffer, int& idx){
    if(idx >= RX_FRAME_BUFFER_LEN){
        fprintf(stderr, "Buffer overflow\n");
        return 0;
    }
    return buffer[idx++];
}
uint16_t PX4Flow::read16(const uint8_t* buffer, int& idx){
    if(idx + 1 >= RX_FRAME_BUFFER_LEN){
        fprintf(stderr, "Buffer overflow\n");
        return 0;
    }
    uint16_t value = 
        (uint16_t)buffer[idx] |
        ((uint16_t)buffer[idx+1] << 8);
    idx+=2;
    return value;
}
uint32_t PX4Flow::read32(const uint8_t* buffer, int& idx){
    if(idx + 3 >= RX_FRAME_BUFFER_LEN){
        fprintf(stderr, "Buffer overflow\n");
        return 0;
    }
    uint32_t low  = read16(buffer, idx);
    uint32_t high = read16(buffer, idx);
    return low | (high << 16);
}










