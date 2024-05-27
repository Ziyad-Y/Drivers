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

static inline int16_t merge_bytes(uint8_t high, uint8_t low){
	return (high << 8) | low;
}

uint16_t read_data(int fd,uint8_t h_addr){
	uint8_t buff[2];   
	buff[0] = h_addr;   
	if(write(fd, buff, 1)!=1){
		perror("Failed to write");
		exit(1);
	}
	if(read(fd, buff, 2)!=2){
		perror("Failed to read");
		exit(1);
	}

	printf("%d %d", buff[0], buff[1]);   
	return merge_bytes(buff[0], buff[1]);
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
   //reset device
   buff[0] = RESET_DEVICE_ADDR;   
   buff[1] = RESET_DEVICE_VAL;
   if(write(fd, buff, 2)!=2){
   	perror("fail write");  
   	exit(1);
   } 
   sleep(1);
   //set FS  
   buff[0]= FS;   
   buff[1]= 0b00010000; //1000 deg/s
   printf("Address 0%x , FS val %d\n", buff[0], buff[1]);
   if(write(fd, buff,2)!=2){
   	perror("Failed to write");
   	exit(1);
   }

   //set AFS 
   buff[0]= AFS;  
   buff[1] = 0;
   printf("Address 0%x , AFS val %d\n", buff[0], buff[1]);
   if(write(fd, buff,2)!=2){
   	perror("Failed to write");
   	exit(1);
   }

   sleep(1);
   
   // enable FIFO  
   buff[0] = FIFO;
   buff[1] = 0b11111000;
    printf("Address 0%x , FIFO val %d\n", buff[0], buff[1]);
   if(write(fd, buff,2)!=2){
   	perror("Failed to write");
   	exit(1);
   }

   sleep(1);

   uint16_t data[4];
   data[0] = read_data(fd, ACCEL_XOUT_H_ADDR);   
   data[1] = read_data(fd, ACCEL_YOUT_H_ADDR); 
   data[2] = read_data(fd, ACCEL_ZOUT_H_ADDR);

   printf("%d %d %d\n", data[0], data[1], data[2]);

    return 0;
}
