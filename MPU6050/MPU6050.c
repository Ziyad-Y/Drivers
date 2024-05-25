#include "MPU6050.h"    

MODULE_AUTHOR("Ziyad");
MODULE_LICENSE("GPL");   
MODULE_DESCRIPTION("A driver for MPU6050 6-Axis Acceloremeter and Gyroscope");      
MODULE_SUPPORTED_DEVICE("NONE");


dev_t device_num;  
static struct class * class;
static struct cdev mydev;



/* Probe the device */
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

static void mpu_remove(struct i2c_client * client){
	pr_info("removing module\n");
	return SUCCESS;
} 


/* I2C Device Creation */
static struct i2c_adadpter mpu6050_adapter = NULL;
static struct i2c_client * client= NULL;
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


