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
	char buffer [5][20];
	struct iovec iov[5];  
	for(int i =0; i < 5;i++){
		iov[i].iov_base = buffer[i];  
		iov[i]. iov_len = sizeof(buffer[i]);
	}

	ssize_t bytes_read = readv(fd, iov, 5);

	if(bytes_read ==0 || bytes_read ==-1){
		perror("ERROR READING FILE or Zero Bytes read\n");
	}  
	for(int i=0; i < 5;i++){
		printf("%0x", strtoull(buffer[i], NULL, 10));
	} 
	close(fd);

	return 0;
}