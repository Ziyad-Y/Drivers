#include <unistd.h>  
#include <stdio.h>   
#include <stdlib.h>   
#include <stdint.h>
#include <fcntl.h>

#define BYTES 28

int main(void){

	int fd = open("/dev/i2c_mpu6000-A", O_RDONLY);
	ssize_t bytesread;
	char buffer[BYTES];

	if(fd ==-1){
		perror("Error openign file");
	}   

	
	bytesread = read(fd,buffer, 28);   
	printf("%s\n", buffer);
	



	return 0;
}