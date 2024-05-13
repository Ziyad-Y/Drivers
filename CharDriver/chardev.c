
#include "chardev.h"
  
MODULE_LICENSE("GPL");              


static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);  // check if device is already opened  
static char BUFF[ BUFFER_SIZE]; // message the device will give   

static struct class * cls;   

static int device_open(struct inode * inode, struct file * file)
{
	static int counter = 0;   
	if(atomic_cmpxchg(&already_open, CDEV_NOT_USED,CDEV_OPEN))
		return -EBUSY;

	sprtintf(BUFF, "Module access = %d\n", counter++);  
	try_module_get(THIS_MODULE);   
	return SUCCESS;
}

static int device_release(struct inode * inode , struct file * file)
{
	atomic_set(&already_open, CDEV_NOT_USED);   
	module_put(THIS_MODULE);
	return SUCCESS;
}

static ssize_t device_read(struct file * flip, char __user * buffer, size_t length, loff_t *  offset)
{
	int bytes_read = 0;   
	const char *buff_ptr = BUFF;   

	if(!*(buff_ptr + *offset)){
		*offset=0;
		return 0;
	}
	BUFF +=*offset;   

	while(length && *buff_ptr){
		put_user(*(buff_ptr++), buffer);
		length--;  
		bytes_read++;
	}
	*offset += bytes_read;   
	return bytes_read;
}

static ssize_t device_write(struct file * flip,const char __user * buffer, size_t length, loff_t *  offset)
{
	pr_alert("Operation not supported \n");
	return -EINVAL;
}
static struct file_operations devfops = {
	.read = device_read,   
	.write = device_write,   
	.open = device_open,   
	.release = device_release
};    



static int major;
static struct class * cls;

static int __init chardev_init(void){
	major = register_chrdev(0, DEVICE_NAME, &devfops );   
	if(major < 0){
		pr_alert("REGISTERING CHAR DEVICE %d\n", major);
		return major;
	}
	pr_info("I was assigned a major number %d\n", major);
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,4,0)  
		csl = class_create(DEVICE_NAME);
	#else  
		cls = class_create(THIS_MODULE, DEVICE_NAME);      
	#endif 
		device_create(cls,NULL,MKDEV(major, 0), NULL, DEVICE_NAME);
		pr_info("Device Created on /dev/%s\n", DEVICE_NAME);   
	return SUCCESS;
}

static void __exit chardev_exit(void)
{
	device_destroy(cls,MKDEV(major,0));   
	class_destroy(cls);   
	unregister_chrdev(major, DEVICE_NAME);
}

