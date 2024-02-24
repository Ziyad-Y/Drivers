#KDIR=/lib/modules/$(uname -r )/build 
KDIR = /lib/modules/6.1.21-v7+/build   
EXTRA_CFLAGS+= -Wall -g 
obj-m +=module.o 

kbuild: 
	make -C $(KDIR) M=$(PWD) modules  

clean:  
	make -C $(KDIR) M=$(PWD) clean
