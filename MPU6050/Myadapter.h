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




#endif