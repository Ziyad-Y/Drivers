
#ifndef MPU6050_H  
#define MPU6050_H
/*
* Device Driver for MPU6050 
* Driver is for raspberry pi 4  
*/

#include <linux/module.h>  
#include <linux/init.h>   
#include <linux/fs.h>  
#include <linux/version.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>  
#include <linux/cdev.h>     
#include <linux/i2c.h>  
#include <linux/slab.h>     
#include <linux/printk.h>
#include <asm/errno.h>
#define SUCCESS 0
#define FAILURE -1
/* MPU6050*/ 
#define DRIVER_NAME "i2c_mpu6000-A"
#define CLASS_NAME "MY_I2C"
#define I2C_BUS 1   					/*I2C bus rawspberry pi (sudo i2cdetect -y 1)*/
#define SLAVE_NAME "MPU6050"
#define SLAVE_ADDRESS 0x68  			/* MPU6050 I2C address */     
/*
*Data Sheet : https://cdn.sparkfun.com/datasheets/Sensors/Accelerometers/RM-MPU-6000A.pdf
*REGISTER MAPPING
*/
#define XTEST 0x0D   
#define YTEST 0x0E
#define ZTEST 0x0F
#define ATEST 0x10

#define ACCEL_XOUT_H_ADDR 0x3B 
#define ACCEL_XOUT_L_ADDR 0x3C     
#define ACCEL_YOUT_H_ADDR 0x3D   
#define ACCEL_YOUT_L_ADDR 0x3E    
#define ACCEL_ZOUT_H_ADDR 0x3F 
#define ACCEL_ZOUT_L_ADDR 0x40   

#define TEMP_OUT_H_ADDR 0x41
#define TEMP_OUT_L_ADDR 0x42

#define GYRO_XOUT_H_ADDR 0x43
#define GYRO_XOUT_L_ADDR 0x44
#define GYRO_YOUT_H_ADDR 0x45   
#define GYRO_YOUT_L_ADDR 0x46  
#define GYRO_ZOUT_H_ADDR 0x47   
#define GYRO_ZOUT_L_ADDR 0x48

/* Full Scale Select Address 
 * AFS +-2g by default   
 * Gyro FS 250 deg/s by default
*/
#define FS 0x1B       
#define AFS 0x1C   

static int mpu_probe(struct i2c_client *, const struct i2c_device_id *);     
static int mpu_release(struct inode *, struct file * );   
static ssize_t read_data(struct file *, char __user *, size_t, loff_t*);   

#endif    

