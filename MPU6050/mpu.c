#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdint.h>

#define MPU6050_ADDRESS 0x68
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B  
#define ACCEL_CONFIG 0x1C
#define FIFO_EN 0x23
#define USER_CTRL 0x6A
#define FIFO_COUNTH 0x72
#define FIFO_COUNTL 0x73   
#define FIFO_R_W 0x74
#define WHO_AM_I 0x75
#define SMPLRT_DIV 0x19
#define  POWER_MGMT 0x6B

#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40

#define TEMP_OUT_H 0x41
#define TEMP_OUT_L 0x42

#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48

static inline uint16_t merger_bytes(uint8_t low, uint8_t high){
	return ((0xFF |high ) << 8) | low;
}


int fd = -1;

void i2c_write(uint8_t reg, uint8_t val){
	uint8_t buf[2];

	if(fd < 0){
		perror("BUS not available");
		exit(1);
	}
	buf[0] = reg;   
	buf[1] = val;   
	if(write(fd, buf , 2)!=2){
		perror("Failed to write buffer");
		exit(1);
	}

}

uint8_t i2c_read(uint8_t reg){
	uint8_t buf[1];
	buf[0] = reg;   
	if(write(fd, buf, 1) !=1){
		perror("Failed to write buffer");
		exit(1);
	}
	if(read(fd, buf, 1)!=1){
		perror("read failed");
		exit(1);
	}
	return buf[0];
}

int16_t two_complement_to_signed(uint8_t low, uint8_t high) {
    uint16_t data = merger_bytes(low, high);   
    if(data & 0x8000) {
        return -(int16_t)(~data + 1);
    } else {
        return data;
    }
}

int main() {

	fd = open("/dev/i2c-22", O_RDWR);   
	if(fd < 0){
		perror("Failed to access bus\n");
		exit(1);
	}
	
	if(ioctl(fd, I2C_SLAVE, MPU6050_ADDRESS) < 0){
		perror("Failed to detect device");
		exit(-1);
	}

	i2c_write(POWER_MGMT, 0x01);
	i2c_write(ACCEL_CONFIG, 0x00);
	i2c_write(GYRO_CONFIG, 0x08);
	i2c_write(CONFIG, 0x00);
	i2c_write(SMPLRT_DIV, 0x07);
	i2c_write(FIFO_EN,0xF8 ); 
	i2c_write(USER_CTRL, 0x44);

	uint8_t ax_l ,ax_h,ay_l ,ay_h, az_l ,az_h, t_l, t_h, gx_l ,gx_h,gy_l ,gy_h, gz_l ,gz_h;  
	float accel_x =0;
	float accel_y =0;
	float accel_z =0; 
	float temp =0;
	float gyro_x =0;
	float gyro_y =0;
	float gyro_z =0;    

	uint16_t FIFO_LEN = 0;  


	while(FIFO_LEN != 1024){
		ax_h = i2c_read(FIFO_COUNTH);
		ax_l = i2c_read(FIFO_COUNTL);
		FIFO_LEN = merger_bytes(ax_l ,ax_h );
		printf("FIFO LEN = %d\n", FIFO_LEN);
		if(FIFO_LEN == 1024){
			i2c_write(USER_CTRL, 0x44);
			continue;
		}

		if(FIFO_LEN >= 14 ){
			ax_h = i2c_read(FIFO_R_W);
			ax_l = i2c_read(FIFO_R_W);
			ay_h = i2c_read(FIFO_R_W);
			ay_l = i2c_read(FIFO_R_W);   
			az_h = i2c_read(FIFO_R_W);
			az_l = i2c_read(FIFO_R_W);
			t_h = i2c_read(FIFO_R_W);  
			t_l = i2c_read(FIFO_R_W);
			gx_h = i2c_read(FIFO_R_W);
			gx_l = i2c_read(FIFO_R_W);
			gy_h = i2c_read(FIFO_R_W);
			gy_l = i2c_read(FIFO_R_W);
			gz_h = i2c_read(FIFO_R_W);
			gz_l = i2c_read(FIFO_R_W);

			accel_x = (float)two_complement_to_signed(ax_l, ax_h)/16384*9.8;   
			accel_y = (float) two_complement_to_signed(ay_l, ay_h)/ 16384 * 9.8;  
			accel_z = (float)two_complement_to_signed(az_l, az_h)/16384 * 9.8;    
			gyro_x =  two_complement_to_signed(gx_l, gx_h)/65.5;   
			gyro_y = two_complement_to_signed(gy_l, gy_h)/65.5;
			gyro_z = two_complement_to_signed(gz_l, gz_h)/65.5;   
			temp = two_complement_to_signed(t_l,t_h)/340.0 + 36.53;  

			printf(" Accel x: %3f, Accel y: %3f, Accel z: %3f\n Gyro x: %3f, Gyro y: %f, Gyro z:%3f\n Temp %3f\n",
				accel_x,accel_y,accel_z,gyro_x,gyro_y,gyro_z,temp);
		}
		else{
			usleep(10000);
		}
	}  





    return 0;
}
