KDIR := /lib/modules/$(shell uname -r)/build   
EXTRA_CFLAGS := -Wall -g 
obj-m  += mymod.o 
mymod-objs := MPU6050.o 
include $(src)/Kconfig

all: 
	make -C $(KDIR) M=$(PWD) modules  

clean:  
	make -C $(KDIR) M=$(PWD) clean
