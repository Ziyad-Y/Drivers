#include "MPU6050.h"    


dev_t device_num;   					/* device number */ 
static struct class * class;			/* device class */
static struct cdev mydev;				/* new character device */
//static s32 * data;						/* data from sensor */

static struct i2c_adapter *adapter = NULL;					/* Adapter */
static struct i2c_client * client= NULL;					/* client */



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
int read_from_MPU6050(struct i2c_client *mpu6050_client, s32 *data) {
    s32 accelx_l, accelx_h, accely_l, accely_h, accelz_l, accelz_h;
    s32 gyrox_l, gyrox_h, gyroy_l, gyroy_h, gyroz_l, gyroz_h;
    s32 tl, th;
    
    /* Reading Acceleration */
    accelx_l = i2c_smbus_read_word_data(mpu6050_client, ACCEL_XOUT_L_ADDR);
    if(accelx_l < 0){
    	pr_info("ERROR READING %d\n", accelx_l);
    	return -EIO;
    }
	accelx_h = i2c_smbus_read_word_data(mpu6050_client, ACCEL_XOUT_H_ADDR);
	if(accelx_h < 0){
    	pr_info("ERROR READING %d\n", accelx_l);
    	return -EIO;
    }
    accely_l = i2c_smbus_read_word_data(mpu6050_client, ACCEL_YOUT_L_ADDR);
    if(accely_l < 0){
    	pr_info("ERROR READING %d\n", accely_l);
    	return -EIO;
    }
    accely_h = i2c_smbus_read_word_data(mpu6050_client, ACCEL_YOUT_H_ADDR);
    if(accely_h < 0){
    	pr_info("ERROR READING %d\n", accely_l);
    	return -EIO;
    }
    accelz_l = i2c_smbus_read_word_data(mpu6050_client, ACCEL_ZOUT_L_ADDR);
    if(accelz_l < 0){
    	pr_info("ERROR READING %d\n", accelz_l);
    	return -EIO;
    }

    accelz_h = i2c_smbus_read_word_data(mpu6050_client, ACCEL_ZOUT_H_ADDR);
    if(accelz_h < 0){
    	pr_info("ERROR READING %d\n", accelz_l);
    	return -EIO;
    }
    data[0] = (accelx_h << 8) | accelx_l;
    data[1] = (accely_h << 8) | accely_l;
    data[2] = (accelz_h << 8) | accelz_l;

    /* Reading Gyroscope Data */
    gyrox_l = i2c_smbus_read_word_data(mpu6050_client, GYRO_XOUT_L_ADDR);
    if(gyrox_l < 0){
    	pr_info("ERROR READING %d\n", gyrox_l);
    	return -EIO;
    }
    gyrox_h = i2c_smbus_read_word_data(mpu6050_client, GYRO_XOUT_H_ADDR);
    if( gyrox_h< 0){
    	pr_info("ERROR READING %d\n", gyrox_h);
    	return -EIO;
    }
    gyroy_l = i2c_smbus_read_word_data(mpu6050_client, GYRO_YOUT_L_ADDR);
    if(gyroy_l < 0){
    	pr_info("ERROR READING %d\n", gyroy_l);
    	return -EIO;
    }
    gyroy_h = i2c_smbus_read_word_data(mpu6050_client, GYRO_YOUT_H_ADDR);
    if( gyroy_h< 0){
    	pr_info("ERROR READING %d\n", gyroy_h);
    	return -EIO;
    }
    gyroz_l = i2c_smbus_read_word_data(mpu6050_client, GYRO_ZOUT_L_ADDR);
    if(gyroz_l < 0){
    	pr_info("ERROR READING %d\n", gyroz_l);
    	return -EIO;
    }
    gyroz_h = i2c_smbus_read_word_data(mpu6050_client, GYRO_ZOUT_H_ADDR);
    if( gyroz_h< 0){
    	pr_info("ERROR READING %d\n", gyroz_h);
    	return -EIO;
    }
    data[3] = (gyrox_h << 8) | gyrox_l;
    data[4] = (gyroy_h << 8) | gyroy_l;
    data[5] = (gyroz_h << 8) | gyroz_l;

    /* TEMPERATURE */
    tl = i2c_smbus_read_word_data(mpu6050_client, TEMP_OUT_L_ADDR);
    if(tl < 0){
    	pr_info("ERROR READING %d\n", tl);
    	return -EIO;
    }
    th = i2c_smbus_read_word_data(mpu6050_client, TEMP_OUT_H_ADDR);
    if( th< 0){
    	pr_info("ERROR READING %d\n", th);
    	return -EIO;
    }
    data[6] = (th << 8) | tl;
    return SUCCESS;
}

static ssize_t read_data(struct file * file, char __user * userbuffer, size_t length, loff_t* offset ){
	int l;
	char buffer[100]; 
	s32 data[7];
	
	  
	if(data ==NULL){
		pr_info("Failed Allocation\n");
		return -ENOMEM;
	}
	if(read_from_MPU6050(client, data) == -EIO){
		pr_info("read failed\n");
		return FAILURE;
	}    
	snprintf(buffer,sizeof(buffer), "%d,%d,%d,%d,%d,%d,%d", data[0],data[1],data[2],data[3],data[4],data[5],data[6] );
	l= strnlen(buffer,90);
	if(copy_to_user(userbuffer, buffer, l) !=0){
		pr_info("Failed to copy data from kernel to user\n");  
		
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
		client = i2c_new_client_device(adapter, &mpu_board_info);
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
	i2c_del_driver(&mpu_driver);   
	cdev_del(&mydev);
	device_destroy(class, device_num);
	class_destroy(class); 
	unregister_chrdev(device_num, DRIVER_NAME);
}

module_init(start);  
module_exit(end_mod);  


MODULE_AUTHOR("Ziyad");
MODULE_LICENSE("GPL");   
MODULE_DESCRIPTION("A driver for MPU6050 6-Axis Acceloremeter and Gyroscope");      

