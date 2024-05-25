#include "MPU6050.h"    

MODULE_AUTHOR("Ziyad");
MODULE_LICENSE("GPL");   
MODULE_DESCRIPTION("A driver for MPU6050 6-Axis Acceloremeter and Gyroscope");      



dev_t device_num;   					/* device number */ 
static struct class * class;			/* device class */
static struct cdev mydev;				/* new character device */
static s32 * data;						/* data from sensor */

static struct i2c_adadpter *adapter = NULL;					/* Adapter */
static struct i2c_client * client= NULL;					/* client */

/* ---------- Probe the device and Remove device ------------- */
static int mpu_probe(struct i2c_client *, const struct i2c_device_id *){
	s32 ret;   
	if (!i2c_check_functionality(client->adapter,
								I2C_FUNC_SMBUS_BYTE_DATA))
			return -EIO; 

	/* Probing addresses to check readibility */
	pr_info("PROBING DATA ADDRESSES\n");

	ret = i2c_smbus_read_byte_data(client, XTEST);   
	if(ret < 0){
		return ret;	
	}

	ret = i2c_smbus_read_byte_data(client, YTEST);     
	if(ret < 0){
		return ret;
	}

	ret = i2c_smbus_read_byte_data(client, ZTEST);   
	if(ret < 0){
		return ret;
	}
	
	/* Write the Full scale select 4g and 1000 deg/s */    
	if( i2c_smbus_write_byte_data(client, AFS, 0x1 ) < 0)
		return FAILURE;    

	if(i2c_smbus_write_byte_data(client, AFS, 0x2) < 0)
		return FAILURE;   
	
	return SUCCESS;
} 
/*Remove Device*/
static void mpu_remove(struct i2c_client * client){
	pr_info("removing module\n");
	kfree(data);
} 

/*----------------Handle File operations-------------- */
static int mpu_open(struct inode * inode, struct file * file){
	pr_info("Opening Device File\n");  
	return SUCCESS;
}
static int mpu_release(struct inode *inode, struct file * file){
	pr_info("Closing Device FIle\n");
	return SUCCESS;
}
/* 
* Read Data
* Data Sheet : https://cdn.sparkfun.com/datasheets/Sensors/Accelerometers/RM-MPU-6000A.pdf  
* Each data from the sensor is 2-bytes (signed)   
* Read both the High and low.
*/
void read_from_MPU6050(struct i2c_client *mpu6050_client, s32 *data) {
    s32 accelx_l, accelx_h, accely_l, accely_h, accelz_l, accelz_h;
    s32 gyrox_l, gyrox_h, gyroy_l, gyroy_h, gyroz_l, gyroz_h;
    s32 tl, th;
    
    /* Reading Acceleration */
    accelx_l = i2c_smbus_read_byte_data(mpu6050_client, ACCEL_XOUT_L_ADDR);
    accelx_h = i2c_smbus_read_byte_data(mpu6050_client, ACCEL_XOUT_H_ADDR);
    accely_l = i2c_smbus_read_byte_data(mpu6050_client, ACCEL_YOUT_L_ADDR);
    accely_h = i2c_smbus_read_byte_data(mpu6050_client, ACCEL_YOUT_H_ADDR);
    accelz_l = i2c_smbus_read_byte_data(mpu6050_client, ACCEL_ZOUT_L_ADDR);
    accelz_h = i2c_smbus_read_byte_data(mpu6050_client, ACCEL_ZOUT_H_ADDR);
    data[0] = (accelx_h << 8) | accelx_l;
    data[1] = (accely_h << 8) | accely_l;
    data[2] = (accelz_h << 8) | accelz_l;

    /* Reading Gyroscope Data */
    gyrox_l = i2c_smbus_read_byte_data(mpu6050_client, GYRO_XOUT_L_ADDR);
    gyrox_h = i2c_smbus_read_byte_data(mpu6050_client, GYRO_XOUT_H_ADDR);
    gyroy_l = i2c_smbus_read_byte_data(mpu6050_client, GYRO_YOUT_L_ADDR);
    gyroy_h = i2c_smbus_read_byte_data(mpu6050_client, GYRO_YOUT_H_ADDR);
    gyroz_l = i2c_smbus_read_byte_data(mpu6050_client, GYRO_ZOUT_L_ADDR);
    gyroz_h = i2c_smbus_read_byte_data(mpu6050_client, GYRO_ZOUT_H_ADDR);
    data[3] = (gyrox_h << 8) | gyrox_l;
    data[4] = (gyroy_h << 8) | gyroy_l;
    data[5] = (gyroz_h << 8) | gyroz_l;

    /* TEMPERATURE */
    tl = i2c_smbus_read_byte_data(mpu6050_client, TEMP_OUT_L_ADDR);
    th = i2c_smbus_read_byte_data(mpu6050_client, TEMP_OUT_H_ADDR);
    data[6] = (th << 8) | tl;
}

static ssize_t read_data(struct file * file, char __user * userbuffer, size_t length, loff_t* offset ){
	data =  kmalloc(6 * sizeof(s32), GFP_KERNEL);   
	if(data ==NULL){
		pr_info("Failed Allocation\n");
		return -ENOMEM;
	}
	read_from_MPU6050(client, data);    

	if(copy_to_user(userbuffer,data, 6 * sizeof(s32)) !=0){
		pr_info("Failed to copy data from kernel to user\n");  
		kfree(data); 
		return -EFAULT;
		
	}
	return SUCCESS;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = mpu_open,  
	.release = mpu_release,  
	.read = read_data
};

/*-----------------I2C Device Creation---------------------*/


static const struct i2c_device_id mpu_id[] = {
		{SLAVE_NAME, 0},  
		{}
};    

static struct i2c_driver mpu_driver = {
		.driver={
			.name = SLAVE_NAME,  
			.owner = THIS_MODULE
		},
		.probe = mpu_probe,  
		.remove = mpu_remove
};

static struct i2c_board_info mpu_board_info = {
	I2C_BOARD_INFO(SLAVE_NAME, SLAVE_ADDRESS)   
};

static int __init start(void){
	if (alloc_chrdev_region(&device_num, 0, 1, DRIVER_NAME) < 0){
		pr_info("cannot allocate device number\n");
	}

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,4,0)
		if((class = class_create(CLASS_NAME)) == NULL){
			pr_info("Cannot create class\n");
			goto class_error;
		}

	#else
		if((class = class_create(THIS_MODULE, CLASS_NAME)) == NULL){
			pr_info("Cannot create class\n");
			goto class_error;
		}
	#endif

	if(device_create(class, NULL, device_num, NULL, DRIVER_NAME)== NULL){
		pr_info("error creating file\n");   
		goto file_error;
	}

	cdev_init(&mydev, &fops);    

	if(cdev_add(&mydev,device_num, 1) == -1){
		pr_info("Failed to add device\n");   
		goto Kerror;
	}

	adapter = i2c_get_adapter(I2C_BUS);    
	
	if( adapter == NULL){
		client = i2c_new_client_device(&adapter, &mpu_board_info);
		if(client == NULL){
			if(i2c_add_driver(&mpu_driver) < 0){
				pr_info("Can't add driver\n");
				return FAILURE;
			}

		}
		i2c_put_adapter(adapter);
	}
	return SUCCESS;
Kerror:  
	device_destroy(class, device_num);   
file_error:  
	class_destroy(class);   

class_error:  
	unregister_chrdev(device_num, DRIVER_NAME);   
	return FAILURE;
}

static void __exit end_mod(void){
	i2c_unregister_device(client);    
	i2c_del_driver(mpu_driver);   
	cdev_del(&mydev);
	device_destroy(class, device_num);
	class_destroy(class); 
	unregister_chrdev(device_num, DRIVER_NAME);
}

module_init(start);  
module_exit(end_mod);