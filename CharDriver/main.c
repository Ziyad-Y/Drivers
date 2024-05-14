#include <stdio.h>  
#include <stdlib.h> 
#include <unistd.h>  
#include <fcntl.h>  


int main(void){

	int fd = open("/dev/mychardev", O_RDONLY);    

	if(fd ==-1 ){
		perror("Failed to open file\n");
		exit(-1);
	}
	printf("Sucess in opening /dev/mychardv\n");
	char BUFF[100];
	ssize_t bytes_read = read(fd, BUFF, 50);   
	if(bytes_read ==0){
		printf("No bytes read\n");    
		return 0;
	}
	printf("Read: %s\n", BUFF);



	return 0;
}