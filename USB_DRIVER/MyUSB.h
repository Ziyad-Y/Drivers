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


#define MIN(a,b)					\
	({								\
		typeof(a) _a =(a);			\
		typeof(b) _b =(b);			\
		_a > b ? _b : _a;			\
	})


#define to_my_usb(ptr) 			\
	container_of(ptr,struct my_usb,kref)   

struct my_usb{
	struct usb_device 		*device;   			/*usb ddevice */
	struct usb_interface 	*interface;   		/* interface of device */
	struct usb_anchor		anchor;				/*anchor*/	
	struct urb 				*bulk_urb;			/*bulk out urb*/   
	struct urb 				*int_urb; 			/*interupt out urb*/ 
	struct semaphore		write_limit; 		/*ensure 64 bytes max at a time*/
	unsigned char 			*wbuff; 			/*buffer that handles writes*/
	unsigned char 			*rbuff;  			/*read data*/
	spinlock_t 				lock;				/*lock*/   
	size_t 					max_out;			/*write size */
	size_t 					max_in; 			/*read size*/  
	struct kref 			kref;  				/*kernel reference*/   
	int 					errors;     		/*save previous errors*/
	u8 						bulk_addr;		/*bulk out address 0x83*/   
	u8						int_addr; 		/*interupt in address 0x82*/   
};



#endif   

