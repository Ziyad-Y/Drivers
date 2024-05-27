#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdlib.h>

#define MPU6050_ADDRESS 0x68

#define FS 0x1B  
#define AFS 0x1C
#define FIFO 0x23

#define RESET_DEVICE_VAL 0x80  
#define RESET_DEVICE_ADDR 0x6B

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


uint8_t read_data(int fd,uint8_t h_addr){
	
	uint8_t buff[1];   
	buff[0] = h_addr;   
	
	if(write(fd, buff, 1)!=1){
		perror("Failed to write");
		exit(1);
	}
	if(read(fd, buff, 2)!=2){
		perror("Failed to read");
		exit(1);
	}

	return buff[0];
}

void mywrite (int fd ,uint8_t * buff, uint8_t addr, uint8_t val){
	buff[0]=addr;  
	buff[1] = val;   
	if(write(fd, buff, 2)!=2){
		perror("failed to write");
		exit(1);
	}

}


int main() {
	int fd;
   	uint8_t buff[2]; 

    // Open the I2C bus
    if ((fd = open("/dev/i2c-1", O_RDWR)) < 0) {
        perror("Failed to open the bus.");
        exit(1);
    }
    //check Device
    if (ioctl(fd, I2C_SLAVE, MPU6050_ADDRESS) < 0) {
        perror("Failed to Access slave");
        exit(1);
    }


   
 
   sleep(1);

   mywrite(fd,buff, FS, 0b10);
   uint8_t d = read_data(fd, FS);
   printf("%d\n", d);

    return 0;
}
