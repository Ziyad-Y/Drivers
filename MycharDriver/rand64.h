#ifndef RAND64 
#define RAND64  

#include <linux/cdev.h>    
#include <linux/device.h>   
#include <linux/atomic.h> //handle race or access
#include <linux/uaccess.h>
#include <linux/version.h>  //contains kernel version
#include <linux/types.h>  
#include <linux/delay.h>    
#include <linux/fs.h> //contains (flip)
#include <linux/kernel.h>
#include <linux/init.h>  // init module
#include <linux/module.h>   
#include <linux/printk.h> //print statements
#include <asm/errno.h>   
#include "xorshiro.h"   
#include <linux/moduleparam.h>

#define SUCCESS 0   
#define DEVICE_NAME "rand64"   


#endif 