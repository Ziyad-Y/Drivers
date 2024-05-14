#ifndef CHARDEV_H   
#define CHARDEV_H    


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


#define SUCCESS 0   
#define DEVICE_NAME "mychardev"   
#define BUFFER_SIZE 100    

enum{
	CDEV_NOT_USED = 0,   
	CDEV_OPEN =1
};


static int device_open(struct inode * , struct file *);   
static int device_release(struct inode *, struct file *);   
static ssize_t device_read(struct file * , char __user *, size_t ,loff_t *);
static ssize_t device_write(struct file * , const char __user *, size_t ,loff_t *);  

#endif 