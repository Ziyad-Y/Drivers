KDIR := /lib/modules/$(shell uname -r)/build   
EXTRA_CFLAGS := -Wall -g 
obj-m  += Myadapter.o  
obj-m  += MPU6050.o

all: 
	make -C $(KDIR) M=$(PWD) modules  

clean:  
	make -C $(KDIR) M=$(PWD) clean
