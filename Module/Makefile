KDIR=/lib/modules/$(uname -r )/build   
EXTRA_CFLAGS+= -Wall -g 
obj-m +=mymod.o  

kbuild: 
	make -C $(KDIR) M=$(pwd) modules   

clean:  
	make -C $(KDIR) M=$(pwd) clean
