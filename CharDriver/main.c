#include <stdio.h>  
#include <stdlib.h> 
#include <unistd.h>  
#include <fcntl.h>  


int main(void){

	int fd = open("/dev/mychardv", O_RDONLY);    

	if(fd ==-1 ){
		perror("Failed to open file\n");
		exit(-1);
	}
	printf("Sucess in opening /dev/mychardv\n");

	return 0;
}