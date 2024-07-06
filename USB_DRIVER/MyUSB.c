#include "MyUSB.h"   


static struct usb_udevice_id usb_table[]= {
	{USB_DEVICE(VENDOR_ID, PRODUCT_ID)},
	{}
};

MODULE_DEVICE_TABLE(usb,usb_table);


/*
* utility functions
* @free_res free resources
*/
static void free_res(struct kref *kref)
{
	struct my_usb *udev = to_my_usb(kref);
	usb_free_urb(udev->int_urb);
	usb_free_urb(udev->bulk_urb);   
	kfree(udev->wbuff);     
	kfree(udev->rbuff);
	usb_put_dev(udev->device);
}

static void wait_stop_all_urbs(struct my_usb *udev)
{
	int timeout = usb_wait_anchor_empty_timeout(&udev->anchor, 1000);

	if(!timeout){
		
		usb_block_urb(udev->bulk_urb);
		usb_block_urb(udev->int_urb);
		usb_kill_anchored_urbs(&udev->anchor);
	}
	usb_kill_urb(udev->bulk_urb);
	usb_kill_urb(udev->int_urb);
}


/*
* File operations fops
*
*/
static void read_callback(struct urb *urb)
{
	struct my_usb *udev;   
	unsigned long flags;

	udev= urb->context;   
	spin_lock_irqsave(&udev->lock, flags);
	if(urb->status){
		if(!(urb->status==-ENOENT||
			 urb->status== -ESHUTDOWN||
			 urb->status==-ECONNRESET)){
			
			dev_err(&udev->interface->dev,
					 "%s: urb error %d\n", __func__, urb->status);
			udev->errors= urb->status;
		}
		
	}
	spin_lock_irqstore(&udev->lock,flags);
}   

static void write_callback(struct urb *urb)
{
	struct my_usb *udev;   
	unsigned long flags;

	udev= urb->context;   
	if(urb->status){
		if(!(urb->status==-ENOENT||
			 urb->status== -ESHUTDOWN||
			 urb->status==-ECONNRESET)){
			
			dev_err(&udev->interface->dev,
					 "%s: urb error %d\n", __func__, urb->status);
		}
		
		spin_lock_irqsave(&udev->lock, flags);
		udev->errors= urb->status;
		spin_lock_irqstore(&udev->lock,flags);
	}
}

static ssize_t write(struct file *file, 
					char __user *buffer, 
					size_t count, 
					loff_t *offset)
{
	struct my_usb *udev;   
	int ret =0;   
	udev= file->private_data;
	size_t write_size = MIN(udev->max_out, count);  

	down(&udev->write_limit);

	if(count==0)
		goto exit;

	udev->bulk_urb = usb_alloc_urb(0,GFP_KERNEL); 
	if(!udev->bulk_urb){
		ret=-ENOMEM;
		goto error;
	}

	udev->wbuff=usb_alloc_coherent(udev->device, write_size,
										GFP_KERNEL,
										&udev->bulk_urb->transfer_dma);

	if(copy_to_user(buffer,udev->wbuff,write_size)){
		ret = -EFAULT;
		goto exit;
	}    

	usb_fill_bulk_urb(udev->bulk_urb,
                  usb_sndbulkpipe(udev->device, udev->bulk_addr),
                  udev->wbuff, write_size,
                  write_callback, 
                  udev);

	udev->bulk_urb->transfer_flags|=URB_NO_TRANSFER_DMA_MAP;   
	usb_anchor_urb(udev->anchor);

	ret= usb_submit_urb(udev->bulk_urb, GFP_KERNEL);

	if(ret){
		dev_err(&udev->interface->dev,  
				"%s :Failed to submit urb, error %d\n", __func__, ret );
		goto anchor_err;
	}

	usb_free_urb(udev->bulk_urb);
	up(&udev->write_limit);
	return write_size;

	anchor_err:
		usb_unanchor_urb(udev->bulk_urb);   

	error:  
		if(udev->bulk_urb){
			usb_free_coherent(udev->device, write_size,
				udev->wbuff,udev->bulk_urb->transfer_dma);
			usb_free_urb(udev->bulk_urb);
		}
		up(&udev->write_limit);    

	exit:   
		return ret;
}
static ssize_t read(struct file *file, 
					const char __user *buffer, 
					size_t count, 
					loff_t *offset)
{
	struct my_usb *udev;
	int ret=0;     
	int interval=10;
	size_t read_size = MIN(udev->max_in, count);    

	udev->private_data;

	if(!udev->int_urb){
		udev->int_urb= usb_alloc_urb(0,GFP_KERNEL);
		if(!udev->int_urb){
			ret=-ENOMEM;
			goto error;
		}
	}

	if(count==0)
		return 0;   

	if(copy_from_user(udev->rbuff,buffer,read_size)){
		ret=-EFAULT;
		goto exit;
	}
	usb_fill_int_urb(udev->int_urb,
					 udev->device,
					 usb_rcvintpipe(udev->device, udev->int_addr), 
					 udev->rbuff,
					 read_size, read_callback,
					 udev,
					 interval
					 );
	usb_anchor_urb(udev->int_urb);

	ret=usb_submit_urb(udev->int_urb, GFP_KERNEL);

	if(ret){
		dev_err(&udev->interface->dev,  
				"%s :Failed to submit urb, error %d\n", __func__, ret );
		goto anchor_err;
	}

	return read_size;

	anchor_err:  
		usb_unanchor_urb(udev->int_urb);   

	error:   
		if(udev->int_urb){
			usb_free_urb(udev->int_urb);
		}
		return ret;   

	exit: return ret;
}
static int open(struct inode *inode, struct file *file){
	struct my_usb *udev;
	struct usb_interface *interface;   
	int minor; 
	int ret=0;

	pr_info("opening udevice"); 
	
	minor = iminor(inode);   
	
	interface = usb_find_interface(&driver,minor);
	
	if(!interface)
		goto error;   

	ret=usb_autopm_get_interface(interface);

	if(!ret)
		goto error;   

	udev=usb_get_intfdata(interface);   

	if(!udev)
		goto error;	
	
	
	kref_get(&udev->kref);

	file->private_data = udev;


	error:
		return ret;
}  
static int release(struct inode *inode, struct file *file)
{
	struct my_usb * udev;
	udev = file->private_data;    

	if(!udev)
		dev_err(&udev->interface->dev, "error accessing udevice");   

	usb_autopm_put_interface(udev->interface);    
	kref_put(&udev->kref, free_res);
}   

static struct file_operations fops={
	.owner=THIS_MODULE,
	.open=open,    
	.release=release,   
	.read-read,
	.write=write,
};

static struct usb_class_driver class={
	.name="myusb-%d",  
	.fops=&fops,
	.minor_base=7
};

/* 
* struct usb_driver
*/
static void disconnect(struct usb_interface *intf)
{
	dev_info(&intf->dev, "disconnecting device");
}    

static int suspend(struct usb_interface *intf, unsigned pm_message_t message)
{
	struct my_usb * udev = usb_get_intfdata(intf);   
		
		if(udev){
			udev_info(&interface->udev, "suspending udevice");
			wait_stop_all_urbs(udev);
			return 0;
		}
		dev_info(&interface->dev, "%s-No device found",__func__ );
		return 0;
}

static int resume(struct usb_interface *intf)
{
	struct my_usb * udev = usb_get_intfdata(intf);
	usb_anchor_resume_wakeups(&udev->anchor);
	return 0;
}
static int probe(struct usb_interface *intf, const struct usb_udevice_id *id){
	int ret = 0;
	struct usb_host_interface *h_interface;   
	struct usb_endpoint_descriptor *e_desc, *bulk_out, *int_in;
	static struct my_usb *udev;
	int i, val;

	if(id->idVendor != VENDOR_ID){
		dev_err(&intf->dev,"Incorrect vendor");
		ret = -1;
	}

	if(id->idProduct != PRODUCT_ID){
		dev_err(&intf->dev,"Incorrect product");
		ret = -1;
	}

	h_interface = interface->cur_altsetting;
	for(i = 0; i< h_interface->desc.bNumEndpoints; i++){
		e_desc = &h_interface->endpoint[i].desc;
		dev_info(&interface->dev,
			"Endpoint:%d\t Address:0x%x\t Max Packet:%d bytes\n", i+1, e_desc->bEndpointAddress, e_desc->wMaxPacketSize);
	}

	udev = kzalloc(sizeof(*udev), GFP_KERNEL);   
	
	if(!udev){
		dev_err(&interface->dev, "Error allocating Memory for Device info");
		ret = -ENOMEM;
		goto error;
	}

	val = usb_find_common_endpoints(intf->cur_altsetting,
									NULL, &bulk_out, &int_in,NULL);
	
	if(val){
		dev_err(&intf->dev, "Could not find endpoints");  
		goto error;
	}
	
	udev->max_out = usb_endpoint_maxp(bulk_out);  
	udev->bulk_addr= bulk_out->bEndpointAddress; 
	udev->max_in = usb_endpoint_maxp(int_in);   	
	udev->int_addr = int_in->bEndpointAddress; 

	
	udev->int_urb = usb_alloc_urb(0, GFP_KERNEL);
	if(!udev->int_urb){	
		ret=-ENOMEM;
		goto error;
	}

	udev->device= usb_get_dev(interface_to_usbdev(intf));
	udev->interface=usb_get_intf(intf);


	init_usb_anchor(&udev->anchor);   
	sema_init(udev->write_limit,1);   
	kref_init(udev->kref);  
	spin_lock_init(&&udev->lock); 

	udev->wbuff = kzalloc(udev_>max_out, GFP_KERNEL);   
	if(!udev->wbuff){
		dev_err(&intf->dev, 
			"Failed to allocate memory for write buffer");
		ret = -ENOMEM;
		goto error; 
	}
	
	udev->rbuff = kzalloc(max_in,GFP_KERNEL);   

	if(!udev->rbuff){
		ret = -ENOMEM;
		goto error;    
	}

	ret = usb_register_dev(intf, &class);

	usb_set_intfdata(intf, udev);
	return ret;
	
	error:
		kref_put(&udev->kref, free_res);
		return ret;
}


static struct usb_driver driver={   
	.probe=probe,
	.suspend=suspend;  
	.disconnect=disconnect,
	.resume=resume,
	.id_table= usb_table
};

static int __init usb_init(void)
{
	int ret;   

	ret= usb_register(&driver);   

	if(ret){
		pr_err("error registering device");
	}
	return ret;
}

static void __exit usb_exit(void);
{
	pr_info("Deregistering device\n");
	usb_deregister(&driver);
}

module_init(usb_init);
module_exit(usb_exit);  

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ziyad Yacoubou");
MODULE_DESCRIPTION("USB DRIVER FOR ARDUINO");
MODULE_VERSION("0.1");