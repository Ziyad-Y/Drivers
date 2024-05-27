#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdlib.h>

#define MPU6050_ADDRESS 0x68

#define ACCEL_XOUT_H_ADDR 0x3B
#define ACCEL_XOUT_L_ADDR 0x3C
#define ACCEL_YOUT_H_ADDR 0x3D
#define ACCEL_YOUT_L_ADDR 0x3E
#define ACCEL_ZOUT_H_ADDR 0x3F
#define ACCEL_ZOUT_L_ADDR 0x40

#define TEMP_OUT_H_ADDR 0x41
#define TEMP_OUT_L_ADDR 0x42

#define GYRO_XOUT_H_ADDR 0x43
#define GYRO_XOUT_L_ADDR 0x44
#define GYRO_YOUT_H_ADDR 0x45
#define GYRO_YOUT_L_ADDR 0x46
#define GYRO_ZOUT_H_ADDR 0x47
#define GYRO_ZOUT_L_ADDR 0x48

int main() {
	int fd;
    

    // Open the I2C bus
    if ((fd = open("/dev/i2c-1", O_RDWR)) < 0) {
        perror("Failed to open the bus.");
        exit(1);
    }

    if (ioctl(fd, I2C_SLAVE, MPU6050_ADDRESS) < 0) {
        perror("Failed to Access slave");
        exit(1);
    }
    uint8_t buff[2]={0};   
    uint8_t addr = 0x4A;
    if(write(fd, &addr, 1)!=1){
    	perror("Failed to write to device");
    	exit(1);
    }

   if(read(fd, buff, 2)!=2){
   	perror("Failed to read");
   	exit(1);
   }
   printf("%d %d\n", buff[0],buff[1]);

   close(fd);
    return 0;
}
