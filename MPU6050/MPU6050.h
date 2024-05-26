
#ifndef MPU6050_H  
#define MPU6050_H
/*
* Device Driver for MPU6050 
* Driver is for raspberry pi 4  
*/

#include <linux/module.h>  
#include <linux/init.h>   
#include <linux/fs.h>  
#include <linux/uaccess.h>
#include <linux/kernel.h>     
#include <linux/i2c.h>  
#include <linux/slab.h>     
#include <linux/printk.h>
#include <asm/errno.h>

#define SUCCESS 0
#define FAILURE -1
/* MPU6050*/ 
#define I2C_BUS 22   					/*I2C bus rawspberry pi (sudo i2cdetect -y 1)*/
#define SLAVE_NAME "MPU6050"
#define SLAVE_ADDRESS 0x68  			/* MPU6050 I2C address or 0x69 if  */     
/*
*Data Sheet : https://cdn.sparkfun.com/datasheets/Sensors/Accelerometers/RM-MPU-6000A. (might be wrong) 
*REGISTER MAPPING
*/
#define FS 0x1B
#define AFS 0x1C
#define XTEST 0x0D   
#define YTEST 0x0E
#define ZTEST 0x0F
#define ATEST 0x10


static int mpuprobe (struct i2c_client *client,const struct i2c_device_id *id);
static int mpuremove (struct i2c_client *client);
#endif    

