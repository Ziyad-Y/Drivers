#include "MyUSB.h"  

static struct usb_driver driver;
static struct usb_class_driver class;


static struct usb_device_id usb_table [] = {
	{USB_DEVICE( VENDOR_ID, PRODUCT_ID )},  
	{}
};    

MODULE_DEVICE_TABLE(usb, usb_table);      

/*Utility Functions*/
void wait_stop_all_urbs(struct my_usb *arduino)
{
	int timeout;   
	timeout = usb_wait_anchor_empty_timeout(arduino->anchor,1000);  
	if(!timeout){
		
		usb_kill_anchored_urbs(&arduino->anchor);
		usb_block_urb(arduino->bulk_out_urb);    
		usb_block_urb(arduino->int_in_urb);
	}
	usb_kill_urb(arduino->bulk_out_urb);  
	usb_kill_urb(arduino->int_in_urb);

}

void complete_stop_urbs(struct my_usb arduino)
{
	
	usb_kill_anchored_urbs(&arduino->anchor);
	usb_kill_urb(arduino->bulk_out_urb);
	usb_kill_urb(arduino->int_in_urb);

}

void clean_up(struct kref * kref)
{
	struct my_usb * arduino = to_my_usb(kref);    
	usb_free_urb(arduino->int_in_urb);
	kfree(arduino->write_buffer);   
	kfree(arduino->read_buffer);
	usb_put_dev(arduino->device);
}


/**** struct usb_driver ****/
static int arduino_probe (struct usb_interface *interface, const struct usb_device_id *id)
{
	int ret = 0;
	struct usb_host_interface *h_interface;   
	struct usb_endpoint_descriptor *e_desc, *bulk_out, *int_in;
	static struct my_usb *arduino;
	int i, val;

	if(id->idVendor != VENDOR_ID){
		dev_err(&interface->dev,"Incorrect vendor");
		ret = -1;
	}

	if(id->idProduct != PRODUCT_ID){
		dev_err(&interface->dev,"Incorrect product");
		ret = -1;
	}

	h_interface = interface->cur_altsetting;
	for(i = 0; i< h_interface->desc.bNumEndpoints; i++){
		e_desc = &h_interface->endpoint[i].desc;
		dev_info(&interface->dev,"Endpoint:%d\t Address:0x%x\t Max Packet:%d bytes\n", i+1, 
										e_desc->bEndpointAddress, e_desc->wMaxPacketSize);
	}

	arduino = kzalloc(sizeof(*arduino), GFP_KERNEL);   
	
	if(!arduino){
		dev_err(&interface->dev, "Error allocating Memory for Device info");
		ret = -ENOMEM;
		goto error;
	}

	val = usb_find_common_endpoints(interface->cur_altsetting,
									NULL, &bulk_out, &int_in,NULL);
	
	if(val){
		dev_err(&interface->dev, "Could not find endpoints");  
		goto error;
	}
	
	arduino->max_out = usb_endpoint_maxp(bulk_out);  
	arduino->bulk_out_addr= bulk_out->bEndpointAddress; 
	arduino->max_in = usb_endpoint_maxp(int_in);   	
	arduino->int_in_addr = int_in->bEndpointAddress; 

	
	arduino->int_in_urb = usb_alloc_urb(0, GFP_KERNEL);
	if(!arduino->int_in_urb){	
		ret=-ENOMEM;
		goto error;
	}

	arduino->device= usb_get_dev(interface_to_usbdev(interface));
	arduino->interface=usb_get_intf(interface);


	init_usb_anchor(&arduino->anchor);   
	sema_init(arduino->write_limit,1);   
	kref_init(arduino->kref);  
	spin_lock_init(&arduino->lock); 

	arduino->write_buffer = kzalloc(arduino_>max_out, GFP_KERNEL);   
	if(!arduino->write_buffer){
		dev_err(&interface->dev, 
			"Failed to allocate memory for write buffer");
		ret = -ENOMEM;
		goto error; 
	}
	
	arduino->read_buffer = kzalloc(max_in,GFP_KERNEL);   

	if(!arduino->read_buffer){
		ret = -ENOMEM;
		goto error;    
	}

	ret = usb_register_dev(interface, &class);
	return ret;
	
	error:
		kref_put(&arduino->kref, clean_up);
		return ret;
}  

static void arduino_disconnect (struct usb_interface * interface)
{
	dev_info(interface->dev, "Disconnecting device");   
}

static int arduino_suspend(struct usb_interface * interface, pm_message_t message)
{
	struct my_usb * dev = usb_get_intfdata(interface);   
	
	if(dev){
		dev_info(interface->dev, "suspending device");
		wait_stop_all_urbs(dev);
		return 0;
	}

	return 0;
}   

static int arduino_resume(struct usb_interface * interface)
{
	struct my_usb * arduino = usb_get_intfdata(interface);
	usb_anchor_resume_wakeups(arduino->anchor);
	return 0;
}  

/**** Communication ****/
static int arduino_open(struct inode * inode, struct file * file)
{
	
	struct my_usb *arduino;
	struct usb_interface *interface;   
	int minor; 
	int ret=0;

	pr_info("opening device"); 
	
	minor = iminor(inode);   
	
	interface = usb_find_interface(&driver,minor);
	
	if(!interface)
		goto error;   

	ret=usb_autopm_get_interface(interface);

	if(!ret)
		goto error;   

	arduino=usb_get_intfdata(interface);   

	if(!arduino)
		goto error;	
	
	
	kref_get(arduino->kref);

	file->private_data = arduino;


	error:
		return ret;
}     

static void arduino_release(struct inode * inode, struct file * file)
{
	struct my_usb * arduino;
	arduino = file->private_data;    

	if(!arduino)
		dev_err(arduino->interface->de, "error accessing device");   

	usb_autopm_put_interface(arduino->interface);    
	kref_put(&arduino->kref, clean_up);
}

static void read_callback(struct urb *urb)
{
	struct my_usb *arduino;
	unsigned long flags;

	arduino->urb->context;

	spin_lock_irqsave(&arduino->lock, flags);

	if(urb->status){
		if(!(urb->status==-ESHUTDOWN||
			 urb->status==-ECONNRESET||
			 urb->status==-ENOENT
			 )){
			dev_err(&arduino->interface->dev,   
				"%s: read failed,error %d\n",
				__func__,urb->status
				);
			arduino->error=urb->status;
		}
	}
	spin_unlock_irqrestore(arduino->lock,flags);


}

static void write_callback(struct urb *urb)
{
	struct my_usb *arduino;
	unsigned long flags
	arduino = urb->context;
	if(urb->status){
		if(!(urb->status== -ENOENT ||
			urb->status== -ECONNRESET ||
			urb->status== -ESHUTDOWN)){

				dev_info(arduino->interface->dev,
				 "%s bulk write error: %d\n", 
				 __func__, urb->status); 
			spin_lock_irqsave(&arduino->lock, flags);
			arduino->error=urb->status;
			spin_unlock_irqrestore(&arduino->lock, flags);
		}
	}

	usb_free_coherent(urb->dev, urb->transfer_buffer_length,
					 urb->transfer_buffer, urb->transfer_dma);
	up(&arduino->write_limit);
}


static ssize_t bulk_transfer_out(struct file *file, 
								const char __user *buffer,
								size_t count, 
								loff_t *offset )
{
	struct my_usb *arduino;   
	int ret =0;   
	arduino= file->private_data;
	size_t write_size = MIN(arduino->max_out, count);  

	down(&arduino->write_limit);

	if(count==0)
		goto exit;

	arduino->bulk_out_urb = usb_alloc_urb(0,GFP_KERNEL); 
	if(!arduino->bulk_out_urb){
		ret=-ENOMEM;
		goto error;
	}

	arduino->write_buffer=usb_alloc_coherent(arduino->device, write_size,
										GFP_KERNEL,
										&arduino->bulk_out_urb->transfer_dma);

	if(copy_to_user(buffer,arduino->write_buffer,write_size)){
		ret = -EFAULT;
		goto exit;
	}    

	usb_fill_bulk_urb(arduino->bulk_out_urb,
					  usb_sndbulkpipe(arduino->device,arduino->bulk_out_addr),
					  arduino->write_buffer, write_size,
					  write_callback,
					  arduino
					  );
	arduino->bulk_out_urb->transfer_flags|=URB_SHORT_TRANSFER_OK;  
	arduino->bulk_out_urb->transfer_flags|=URB_TRANSFER_DMA_MAP;   
	usb_anchor_urb(arduino->anchor);

	ret= usb_submit_urb(arduino->bulk_out_urb, GFP_KERNEL);

	if(ret){
		dev_err(&arduino->interface->dev,  
				"%s :Failed to submit urb, 
				error %d\n", __func__, ret );
		goto anchor_err;
	}

	usb_free_urb(arduino->bulk_out_urb);
	up(&arduino->write_limit);
	return write_size;

	anchor_err:
		usb_unanchor_urb(arduino->bulk_out_urb);   

	error:  
		if(arduino->bulk_out_urb){
			usb_free_coherent(arduino->device, write_size,
				arduino->write_buffer,arduino->bulk_out_urb->transfer_dma);
			usb_free_urb(arduino->bulk_out_urb);
		}
		up(&arduino->write_limit);    

	exit:   
		return ret;
}    


static ssize_t interupt_transfer_in(struct file * file, 
									char __user * buffer, 
									size_t count, 
									loff_t * offset )
{
	struct my_usb *arduino;
	int ret=0;     
	int interval=10;
	size_t read_size = MIN(arduino->max_in, count);    

	arduino->private_data;

	if(!arduino){
		arduino->int_in_urb= usb_alloc_urb(0,GFP_KERNEL);
		if(!arduino){
			ret=-ENOMEM;
			goto error;
		}
	}

	if(count==0)
		return 0;   

	if(copy_from_user(arduino->read_buffer,buffer,read_size)){
		ret=-EFAULT;
		goto exit;
	}
	usb_fill_int_urb(arduino->int_in_urb,
					 arduino->device,
					 usb_rcvintpipe(arduino->device, arduino->int_in_addr), 
					 arduino->read_buffer,
					 read_size, read_callback,
					 arduino,
					 interval
					 );
	usb_anchor_urb(arduino->int_in_urb);

	ret=usb_submit_urb(arduino->int_in_urb, GFP_KERNEL);

	if(ret){
		dev_err(&arduino->interface->dev,  
				"%s :Failed to submit urb, 
				error %d\n", __func__, ret );
		goto anchor_err;
	}

	return read_size;

	anchor_err:  
		usb_unanchor_urb(arduino->int_in_urb);   

	error:   
		if(arduino->int_in_urb){
			usb_free_urb(arduino->int_in_urb);
		}
		return ret;   

	exit:   
		return ret;

}

static const struct file_operations fops= {
	.owner=THIS_MODULE,   
	.open=arduino_open,
	.release=arduino_release, 
	.read=bulk_transfer_out,    
	.write= interupt_transfer_in
};

class = {
	.name = "usb-%d",   
	.fops=fops,
	.minor_base=7

};

driver = 
{
	.name="ard_usb",   
	.probe= arduino_probe,
	.disconnect = arduino_disconnect,    
	.suspend=arduino_suspend,   
	.resume= arduino_resume, 
	.id_table= usb_table
};


static int __init start_mod(void)
{
	int ret;   
	ret= usb_register(&driver);
	if(ret< 0){
		pr_info("Error Initiating Driver");
		return -1;
	}

	return ret;


}

static void __exit end_mod(void)
{
	pr_info("Removing MOD\n");
	usb_deregister(&driver);
}

module_init(start_mod);   
module_exit(end_mod);  

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ziyad");
MODULE_DESCRIPTION("USB DRIVER FOR ARDUINO");
MODULE_VERSION("0.0.1");