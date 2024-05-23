KDIR := /lib/modules/$(shell uname -r)/build   
EXTRA_CFLAGS := -Wall -g 
include $(src)/Kconfig
obj-m  += mymod.o 
mymod-objs := rand64.o 

all: 
	make -C $(KDIR) M=$(PWD) modules  

clean:  
	make -C $(KDIR) M=$(PWD) clean
