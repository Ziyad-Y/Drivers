/*
* Creating I2C device adapter  
* Adapter managees communication process between slave and masters
*/

#ifndef MY_ADAPTER_H   
#define MY_ADAPTER_H  
#include <linux/kernel.h>     
#include <linux/i2c.h>  
#include <linux/slab.h>
#include <linux/module.h>  
#include <linux/init.h> 
#include <asm/errno.h>   
#include <linux/delay.h>

#define ADAPTER_NAME "MPU6050_ADAPTER"   
#define SUCCESS 0  
#define FAILURE -1

/*Algortihms function of adapter struct*/
int my_xfer(struct i2c_adapter * adapter , struct i2c_msg * msgs, int num );   
int my_smbus_xfer(struct i2c_adapter * adapter, u16 addr , unsigned short flags , 
											char read_write, 
											u8 command , int size, union i2c_smbus_data * data);   
u32 my_func(struct i2c_adapter *adapter);    


#endif