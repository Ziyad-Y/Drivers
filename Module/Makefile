#KDIR=/lib/modules/$(uname -r )/build 
CONFIG_PLATFORM_ARM_RPI = y
CONFIG_PLATFORM_ARM64_RPI = n

KDIR = /lib/modules/6.1.21-v7+/build   
EXTRA_CFLAGS+= -Wall -g 
obj-m +=module.o 

kbuild: 
	make -C $(KDIR) M=$(PWD) modules  

clean:  
	make -C $(KDIR) M=$(PWD) clean
