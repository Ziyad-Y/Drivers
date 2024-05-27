#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
int main(void){
	int file;
	char filename[20];
	int addr = 0x68; // I2C address of the MPU
	uint16_t dataAddr = 0x3B; // Data address to read from
	uint8_t val;
	
	sprintf(filename, "/dev/i2c-%d", 0); // Use the appropriate I2C bus number
	if ((file = open(filename, O_RDWR)) < 0)
	    exit(1);
	
	if (ioctl(file, I2C_SLAVE, addr) < 0)
	    exit(2);
	
	if (write(file, &dataAddr, 2) != 2) {
	    exit(3);
	}
	
	if (read(file, &val, 1) != 1) {
	    exit(3);
	}
	// 'val' now contains the read data
	return 0;
}