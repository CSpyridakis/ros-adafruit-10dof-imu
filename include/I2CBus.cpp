#include "ros/ros.h"
#include "I2CBus.h"
#include <unistd.h>
#include <fcntl.h>

// run sudo apt-get install libi2c-dev
#include <sys/ioctl.h>
extern "C" {
    #include <linux/i2c.h>
    #include <linux/i2c-dev.h>
    #include <i2c/smbus.h>
}

// i2cdetect -y 1
I2CBus::I2CBus(const char * deviceName){
    fd = open(deviceName, O_RDWR);
    if (fd == -1){
        ROS_FATAL("Failed to open I2C device %s", deviceName);
        throw posix_error(std::string("Failed to open I2C device ") + deviceName);
    }
    else{
        ROS_INFO("I2C device [%s] opened successfully (fd: %d)", deviceName, fd);
    }
}

I2CBus::~I2CBus(){
    close(fd);
}

void I2CBus::addressSet(uint8_t address){
    char hex_string[20];
    sprintf(hex_string, "0x%X", address);

    int result = ioctl(fd, I2C_SLAVE, address);
    if (result == -1){
        ROS_FATAL("Failed to set address");
        throw posix_error("Failed to set address");
    }
    else{
        ROS_INFO("I2C device address setted up [%s] (result: %d)", hex_string, result);
    }
}

void I2CBus::writeByte(uint8_t command, uint8_t data){
    char comm_hex[20];
    char data_hex[20];
    sprintf(comm_hex, "0x%X", command);
    sprintf(data_hex, "0x%X", data);

    int result = i2c_smbus_write_byte_data(fd, command, data);
    if (result == -1){
        ROS_FATAL("Failed to write byte to I2C");
        throw posix_error("Failed to write byte to I2C");
    }
    else{
        ROS_INFO("Write Byte command [%s] data [%s] (result: %d)", comm_hex, data_hex, result); 
    }
}

uint8_t I2CBus::readByte(uint8_t command){
    char comm_hex[20];
    sprintf(comm_hex, "0x%X", command);

    int result = i2c_smbus_read_byte_data(fd, command);
    if (result == -1){
        ROS_FATAL("Failed to read byte from I2C");
        throw posix_error("Failed to read byte from I2C");
    }
    else{
        ROS_INFO("Read byte command [%s] (result: %d)", comm_hex, result); 
    }
    return result;
}

int I2CBus::tryReadByte(uint8_t command){
    char comm_hex[20];
    sprintf(comm_hex, "0x%X", command);

    return i2c_smbus_read_byte_data(fd, command);
}

void I2CBus::readBlock(uint8_t command, uint8_t size, uint8_t * data){
    char comm_hex[20];
    char data_hex[20];
    sprintf(comm_hex, "0x%X", command);
    sprintf(data_hex, "0x%X", *data);

    int result = i2c_smbus_read_i2c_block_data(fd, command, size, data);
    if (result != size){
        ROS_FATAL("Failed to read block from I2C");
        throw posix_error("Failed to read block from I2C");
    }
    // else{
    //     ROS_INFO("Read block command [%s] data [%s] (result: %d)", comm_hex, data_hex, result); 
    // }
}
