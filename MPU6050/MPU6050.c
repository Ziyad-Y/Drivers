#include "MPU6050.h"    


/*-----------------I2C Device Creation---------------------*/
static struct i2c_adapter * adapter = NULL;  
static struct i2c_client * client = NULL;   

/*
* Write to slave device
*/

static int I2C_Write(u8 * buf, unsigned int len){
	int ret;   

	ret = i2c_master_send(client, buf, len);

	if(ret < 0){
		pr_info("failed to write\n");   
		return FAILURE;
	}

	return SUCCESS;

}

/*
* Read data from slave device
*/

static int I2C_Read(u8 *outbuf, unsigned int len){
	int ret;   

	ret = i2c_master_recv(client, outbuf, len);   

	if(ret < 0){
		pr_info("Did not receive\n");
		return FAILURE;
	}

	return ret;
}

/*
*set the full scal for acceleration and gyro
*/
static void set_full_scale(u8 address , u8 fs_val){

	u8 buf [2] = {address, fs_val};   

	if(I2C_Write(buf,2 ) < 0){
		pr_info("FAILED TO WRITE TO DATA\n");
		
	}

}

/*read the test values*/
static void read_test_val(void){
	int b1,b2,b3;
	u8 buf1[1] = {XTEST};
	u8 buf2[1] = {YTEST};   
	u8 buf3[1] = {ZTEST};      

	b1 = I2C_Read(buf1, 1);   
	if(b1 < 0)
		pr_info("Read failed\n");  
	else
		pr_info("address 0x%x Data 0x%x\n", XTEST, buf1[1]);    
	b2 = I2C_Read(buf2, 1);
	if(b2 < 0)
		pr_info("Read failed\n");  
	else
		pr_info("address 0x%x Data 0x%x\n", YTEST, buf2[1]);  
	b3=  I2C_Read(buf3, 1);   
	if(b3 < 0)
		pr_info("Read failed\n");  
	else
		pr_info("address 0x%x Data 0x%x\n", ZTEST, buf3[1]);
}

static int mpuprobe(struct i2c_client *client) {
    set_full_scale(FS, 2);
    set_full_scale(AFS, 1);
    read_test_val();
    return SUCCESS;
}

static void mpuremove (struct i2c_client *client){
	pr_info("removing device\n");
}


static const struct i2c_device_id mpu_id[] = {
	{SLAVE_NAME, 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, mpu_id);   

/*Create Driver Struct*/
static struct i2c_driver mpu_driver = {
	.driver = {
		.name = SLAVE_NAME,  
		.owner = THIS_MODULE
	},
	.probe = mpuprobe,   
	.remove = mpuremove,
	.id_table = mpu_id
};
/*Board struct*/

static struct i2c_board_info mpu_board_info = {
	I2C_BOARD_INFO(SLAVE_NAME, SLAVE_ADDRESS)   
};    

static int __init mpu_init(void){
	adapter = i2c_get_adapter(I2C_BUS);   
	if(adapter != NULL){
		client = i2c_new_client_device(adapter, &mpu_board_info);   
		if(client != NULL ){
			i2c_add_driver(&mpu_driver);
		}
		i2c_put_adapter(adapter);
	}
	pr_info("Client Driver added\n");
	return SUCCESS;
}

static void __exit mpu_exit(void){
	i2c_unregister_device(client);   
	i2c_del_driver(&mpu_driver);   
	pr_info("Driver removed\n");
}


module_init(mpu_init);
module_exit(mpu_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ziyad");
MODULE_DESCRIPTION("Driver for MPU6050");
MODULE_VERSION("0.1");      

