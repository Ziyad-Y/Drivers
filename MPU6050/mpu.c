#include <linux/i2c-dev.h>  
#include <stdio.h>  
#include <sys/ioctl.h>  
#include <fcntl.h>  
#include <unistd.h>  
#include <stdint.h>   
#include <errno.h>

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

static inline int32_t merge_bytes(int8_t high, int8_t low){
	return (high << 8) | low;
} 






int main(void){
	int i2c_bus;   
	
	i2c_bus = open("/dev/i2c-1",O_RDWR);   

	if(i2c_bus < 0){
		perror("failed to opening bus");   
		return -1;
	}

	if(ioctl(i2c_bus, I2C_SLAVE, 0x68) < 0){
		perror("error getting slave address");
		return -1;
	}

	uint8_t data[7];   

	data[0]= ACCEL_XOUT_H_ADDR; 
	if(write(fd, data, 1) != 1){
		perror("Failed to write");
		return -1;
	}

	if(read(fd, data, 2) != 2 ){
		perror("Failed to read");
		return -1;
	}  

	printf("data 0x%x 0x%x\n", data[0],data[1]);


	



	return 0;
}