#include "MyUSB.h"   


/*
* utility functions
* @free_res free resources
*/
void free_res(struct kref *kref);
void wait_stop_all_urbs(struct my_usb *usbdev);

/*
* File operations fops
*/
ssize_t read(struct file *, char __user *, size_t, loff_t *);
ssize_t write(struct file *, const char __user *, size_t, loff_t);
int open(struct inode *, struct file);   
int release(struct inode * , struct file *);   

/* 
* struct usb_driver
*/
void disconnect(struct usb_interface *intf);    
int suspend(struct usb_interface *intf, unsigned pm_message_t message);
int resume(struct usb_interface *intf);
int probe(struct usb_interface *intf, const struct usb_device_id *id);


