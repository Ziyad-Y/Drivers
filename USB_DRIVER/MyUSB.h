/*
* Driver Communication with Arduino R3  
* One way data streaming. Either sending Data or Receiving. Communication is not Asynchronus
*/

#ifndef USB_HEAD
#define USB_HEAD

/***** HEADERS *****/   
#include <linux/init.h>  
#include <linux/module.h>  
#include <linux/kernel.h>   
#include <linux/slab.h>   
#include <asm/errno.h>  
#include <linux/usb.h>
#include <linux/uaccess.h>
#include <linux/semaphore.h>
#include <linux/spinlock.h>
#include <linux/printk.h>   
#include <linux/types.h>
#include <linux/kref.h>
#include <linux/fs.h>

/* Arduino R3 */
#define VENDOR_ID 0x2341  /* vendor idendtifier*/   
#define PRODUCT_ID 0x0043  /* product id */
#define MAX_PACKET_OUT 0x0040 /* 1x 64 bytes*/    
#define MAX_PACKET_IN 0x0008 /* 1x 8 bytes */


#define MIN(a,b)					\
	({								\
		typeof(a) _a =(a);			\
		typeof(b) _b =(b);			\
		_a > b ? _b : _a;			\
	})


#define to_my_usb(d) 			\
	container_of(ptr,struct my_usb,kref)   

struct my_usb{
	struct usb_device 		*device;   			/*usb ddevice */
	struct usb_interface 	*interface;   			/* interface of device */
	struct usb_anchor		anchor;
	struct urb 				*bulk_out_urb;		/*bulk out urb*/   
	struct urb 				*int_in_urb; 		/*interupt out urb*/ 
	struct semaphore		write_limit; 		/*ensure 64 bytes max at a time*/
	unsigned char 			*write_buffer; 		/*buffer that handles writes*/
	unsigned char 			*read_buffer;  		/*read data*/
	spinlock_t 				lock;				/*lock*/   
	size_t 					max_out;			/*write size */
	size_t 					max_in 				/*read size*/  
	struct kref 			kref;  				/*kernel reference*/   
	int 					errors;     		/*save previous errors*/
	u8 						bulk_out_addr;		/*bulk out address 0x83*/   
	u8						int_in_addr; 		/*interupt in address 0x82*/   

};


/*
* struct usb_driver   
* @probe probe all usb devices and find flashdrive    
* @disconnect unplug/disconnect usb device from machine   
* @suspend called when Device is going to suspended by system   
* @resume called when device is being resumed by system
*/

int arduino_probe (struct usb_interface *interface, const struct usb_device_id *id);   

void arduino_disconnect (struct usb_interface *interface);  

int arduino_suspend(struct usb_interface *interface, pm_message_t message);   

int arduino_resume(struct usb_interface *interface);

/*
* Utility function
* @wait_stop_all_urbs wait a time duration and kill urb
* @complete_stop_urbs kill all urb at once without wating.
* @resume_urb resume urb request and communication after suspend
* @clean_up free up resources that are no longer being used  
*/

void wait_stop_all_urbs(struct my_usb *arduino);

void complete_stop_urbs(struct my_usb * arduino);

void resume_urb(struct my_usb *arduino);

void clean_up(struct my_usb *arduino);

void init_my_usb(struct my_usb * arduino);
/*  
* Establish communication with arduino
* IN : Interupt Transfer  
* OUT : BULK Transfer
* @arduino_open needed for fops  
* @arduino_release needed for fops
* @bulk_trasnfer_out tranfer data from HOST to device connected Via USB  
* @write_callback write callback functionf for urb
* @interupt_tranfer_in read data from USB
* @read_callback  read callback function
*/
int arduino_open(struct inode *inode, struct file *file);     

void arduino_release(struct inode *inode, struct file *file)

ssize_t bulk_transfer_out(	struct file * file, 
							const char __user * buffer, 
							size_t count, 
							loff_t *offset );   
void write_callback(struct urb *urb);

ssize_t interupt_transfer_in(struct file *file, 
							char __user *buffer, 
							size_t count, 
							loff_t *offset );   

void read_callback(struct urb *urb);

#endif   

