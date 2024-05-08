#KDIR=/lib/modules/$(uname -r )/build 
KDIR := /lib/modules/$(shell uname -r)/build   
EXTRA_CFLAGS := -Wall -g 
obj-m := mymod.o 
#module-objs := file1.o file2.o file3.o

all: 
	make -C $(KDIR) M=$(PWD) modules  

clean:  
	make -C $(KDIR) M=$(PWD) clean
