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

static inline int16_t merge_bytes(int8_t high, int8_t low){
	return (high << 8) | low;
} 

uint8_t buffer[4];

int16_t read_data(int fd, uint8_t high_address, uint8_t low_address){
	uint8_t low, high;
	buffer[0] = high_address;
	if(write(fd, buffer, 1) != 1){
		perror("Failed to write");
		return -1;
	}

	if(read(fd, buffer, 2) != 2 ){
		perror("Failed to read");
		return -1;
	}
	
	high=buffer[0];   

	buffer[0]=low_address;
	if(write(fd, buffer, 1) != 1){
		perror("Failed to write");
		return -1;
	}
	if(read(fd, buffer, 2) != 2){
		perror("Failed to read");
		return -1;
	}

	return merge_bytes(high, low);

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

	uint16_t data[7];    

	data[0] = read_data(i2c_bus, ACCEL_XOUT_H_ADDR, ACCEL_XOUT_L_ADDR);   
	data[1] = read_data(i2c_bus, ACCEL_YOUT_H_ADDR, ACCEL_YOUT_L_ADDR);  
	data[2] = read_data(i2c_bus, ACCEL_ZOUT_H_ADDR, ACCEL_ZOUT_L_ADDR);   
	data[3] = read_data(i2c_bus, GYRO_XOUT_H_ADDR, GYRO_XOUT_L_ADDR);   
	data[4] = read_data(i2c_bus, GYRO_YOUT_H_ADDR, GYRO_YOUT_L_ADDR);   
	data[5] = read_data(i2c_bus, GYRO_ZOUT_H_ADDR, GYRO_ZOUT_L_ADDR);
	data[6] = read_data(i2c_bus, TEMP_OUT_H_ADDR, TEMP_OUT_L_ADDR);   

	printf(" accelx: 0x%x, gyrox 0x%x, temp 0x%x\n", data[0], data[3], data[6]);



	return 0;
}