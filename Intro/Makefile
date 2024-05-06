#KDIR=/lib/modules/$(uname -r )/build 
KDIR = /lib/modules/$(shell uname -r)/build   
EXTRA_CFLAGS+= -Wall -g 
obj-m +=mymod.o 

all: 
	make -C $(KDIR) M=$(PWD) modules  

clean:  
	make -C $(KDIR) M=$(PWD) clean
