#include <linux/i2c-dev.h>  
#include <stdio.h>  
#include <sys/ioctl.h>  
#include <fcntl.h>  
#include <unistd.h>  
#include <stdint.h>   
#include <errno.h>

#define MPU6050_ADDRESS 0x68
#define ACCEL_XOUT_H_ADDR 0x3B

static inline int16_t merge_bytes(uint8_t high, uint8_t low){
    return (int16_t)((high << 8) | low);
} 

int main(void){
    int i2c_bus;   
    int16_t accel_x;
    
    i2c_bus = open("/dev/i2c-1", O_RDWR);   

    if (i2c_bus < 0){
        perror("Failed to open bus");   
        return -1;
    }

    if (ioctl(i2c_bus, I2C_SLAVE, MPU6050_ADDRESS) < 0){
        perror("Error setting slave address");
        return -1;
    }

    uint8_t reg_addr = ACCEL_XOUT_H_ADDR;
    if (write(i2c_bus, &reg_addr, sizeof(reg_addr)) != sizeof(reg_addr)){
        perror("Failed to write register address");
        return -1;
    }

    uint8_t data[2]; // We need 2 bytes for X-axis

    if (read(i2c_bus, data, sizeof(data)) != sizeof(data)){
        perror("Failed to read data");
        return -1;
    }

    accel_x = merge_bytes(data[0], data[1]);

    printf("Accelerometer data (X-axis): %d\n", accel_x);

    close(i2c_bus);

    return 0;
}