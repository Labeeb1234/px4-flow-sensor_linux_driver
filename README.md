# PX4-Flow Sensor Doc
---

## Main Docs
- [PX4_FLOW-v.1.3.1](https://docs.px4.io/v1.13/en/sensor/px4flow.html)
- [px4-flow-sensor driver for esp32 and arduino](https://github.com/eschnou/arduino-px4flow-i2c)
- The pinouts for esp32 and raspi 4b give below



- **Note**: The sensor support is depreciated as of now.


<div>
    <img src="https://github.com/user-attachments/assets/e6cfe17a-41a4-4bfe-9f6c-bc25096ffdbe" alt="raspi-4b pinout" />
    <img src="https://mischianti.org/wp-content/uploads/2021/02/ESP32-wroom-32-pinout-mischianti-high-resolution.png" alt="esp32-wroom-32U_pinout" />
</div>


**(READ TILL HERE)**
## -------


## Generic Linux Driver (No need to look at this for now)

- Not exactly a fully fledged driver yet there is only one test script that displays the data
- Need to be improved for usage not required



## VIO-Implementation (Improve Robot Base Localization )

- px4_flow sensor integration
	- Base Baud rate of sensor: 115200
	- Need to create a driver (i2c based driver)
	- Serial drivers only provide the image frames from the optical sensor
	- Need to check feasibility of integration




