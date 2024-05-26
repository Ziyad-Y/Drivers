#include <linux/i2c-dev.h>  
#include <stdio.h>  
#include <sys/ioctl.h>  
#include <fcntl.h>  
#include <unistd.h>  
#include <stdint.h>   
#include <errno.h>

#define ACCEL_XOUT_H_ADDR 0x3B
#define ACCEL_YOUT_H_ADDR 0x3D
#define ACCEL_ZOUT_H_ADDR 0x3F

static inline int16_t merge_bytes(int8_t high, int8_t low){
    return (high << 8) | low;
} 

int main(void){
    int i2c_bus;   
    int16_t accel_x, accel_y, accel_z;
    
    i2c_bus = open("/dev/i2c-1", O_RDWR);   

    if (i2c_bus < 0){
        perror("failed to open bus");   
        return -1;
    }

    if (ioctl(i2c_bus, I2C_SLAVE, 0x68) < 0){
        perror("error setting slave address");
        return -1;
    }

    int8_t data[6]; // We need 6 bytes for X, Y, and Z axes

    // Reading accelerometer data
    data[0] = ACCEL_XOUT_H_ADDR; 
    if (write(i2c_bus, data, 1) != 1){
        perror("Failed to write");
        return -1;
    }

    if (read(i2c_bus, data, 6) != 6){
        perror("Failed to read");
        return -1;
    }

    // Merge high and low bytes for each axis
    accel_x = merge_bytes(data[0], data[1]);
    accel_y = merge_bytes(data[2], data[3]);
    accel_z = merge_bytes(data[4], data[5]);

    printf("Accelerometer data: X=%d, Y=%d, Z=%d\n", accel_x, accel_y, accel_z);

    return 0;
}
