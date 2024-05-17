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
	
	char buffers [10][20];   
	struct iovec iov[10];    
	
	for(int i =0 ; i< 10; i++){
		iov[i].iov_base = buffers[i];   
		iov[i].iov_len = sizeof(buffers[i]);
	}
	ssize_t bytes_read = readv(fd, &iov, 10);
	if(bytes_read == 0){
		perror("Zero bytes read\n");  
		exit(-1);
	}
	else if(bytes_read == -1){
		perror("Error reading bytes");
		exit(-1);
	}

	for(int i = 0; i< 10;i++){
		printf("%lld\n", strtoull(buffers[i], NULL, 10));
	}
	return 0;
}