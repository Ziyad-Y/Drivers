/*
* Create a Device File /dev/rand64   
* Char Device File will output stream of 64-bit random numbers  
* Numbers will be generated using xorshiro128++ 
*/
#ifndef RAND64 
#define RAND64  
#include <linux/cdev.h>    
#include <linux/device.h>   
#include <linux/atomic.h> //handle race or access
#include <linux/uaccess.h> // put_user
#include <linux/version.h>  //contains kernel version
#include <linux/types.h>  
#include <linux/delay.h>    
#include <linux/fs.h> //contains (flip)
#include <linux/kernel.h>
#include <linux/init.h>  // init module
#include <linux/module.h>   
#include <linux/printk.h> //print statements
#include <asm/errno.h>   
#include <linux/moduleparam.h>
#include <linux/string.h>

#define SUCCESS 0   
#define DEVICE_NAME "rand64"   

struct xorshiro
{
	u64 SEED;
	u64 s[2];   
	int jump;
};

static void generator_init(struct xorshiro* ,u64, int);  
static u64 next(struct xorshiro *);   
static void jump(struct xorshiro *);   
static int device_open(struct inode * , struct file *);   
static int device_release(struct inode *, struct file *);   
static ssize_t device_read(struct file * , char __user *, size_t ,loff_t *);

enum{
	UNUSED =0,
	OPENED =1   	
};
#endif 