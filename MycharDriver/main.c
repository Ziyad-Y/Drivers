#include <unistd.h>  
#include <sys/uio.h>   
#include <fcntl.h> 
#include <stdio.h>  
#include <stdlib.h>

int main(void){
	int fd = open("/dev/rand64", O_RDONLY);    

	if(fd ==-1){
		perror("Failed to read file \n");    
		exit(-1);
	}
	char buffer [20];   

	ssize_t bytes_read = read(fd, buffer, sizeof(buffer));   

	if(bytes_read == 0){
		perror("Read 0 Bytes\n");  
		exit(-1);
	}
	else if (bytes_read ==-1){
		perror("Error reading file");
		exit(-1);
	}

	printf("Read : %lld\n", strtoull(buffer, NULL, 10));

	close(fd);

	return 0;
}