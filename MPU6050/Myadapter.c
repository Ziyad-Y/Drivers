#include "Myadapter.h"   

/*
* Link bellow has all the functionalities
* https://www.kernel.org/doc/html/latest/i2c/functionality.html
* functionality the adpater is capable of
*/
static u32 my_func(struct i2c_adapter * adpter){
	return ( I2C_FUNC_I2C | I2C_FUNC_SMBUS_QUICK |
			 I2C_FUNC_SMBUS_READ_BYTE | I2C_FUNC_SMBUS_WRITE_BYTE |
			 I2C_FUNC_SMBUS_READ_BYTE_DATA | I2C_FUNC_SMBUS_WRITE_BYTE_DATA |   
			 I2C_FUNC_SMBUS_READ_WORD_DATA | I2C_FUNC_SMBUS_READ_BLOCK_DATA
			)
}   

/*
* my_xfer handles transaction made to the adapter
* invoke when device driver needs to communicate with I2C device.
* i2c_mesg structs   
* struct i2c_msg {
	__u16 addr;					// slave address
	__u16 flags;				// write 0 , read flags | M_RD, 10bit address flag | M_TEN 
	#define I2C_M_TEN 0x0010
	#define I2C_M_RD 0x0001
	__u16 len;					// size of buffer
	__u8 * buf;					// buffer
		};
*/

static int my_xfer(struct i2c_adapter * adapter, struct i2c_msg * msgs, int num ){
	int i;
	for( i =0; i< num; i++){
		struct i2c_msg * temp = & msgs[i];
		pr_info("[Message Number : %d] [%s] : [Addr = 0x%x] [len=%d]\n", i+1, __func__,temp->addr,temp->len);
	}
	return SUCCESS;

} 
/*
* smbus_xfer is used when i2c chip wants to communicate with the chip device using smbus protocol
*/
static int my_smbus_xfer(	struct i2c_adapter *adapter, 
							u16 addr, 
							unsigned short flags , 
							char read_write, 
							u8 command, 
							int size, 
							union i2c_smbus_data * data
							)
{
	pr_info("Func : %s", __func__);
	return SUCCESS;
}  


static struct i2c_algorithm myalgo {
	.master_xfer = my_xfer,   
	.smbus_xfer = my_smbus_xfer,  
	.functionality = my_func
};


static struct i2c_adapter adapter ={
	.owner = THIS_MODULE,
	.name = ADAPTER_NAME
	.class = I2C_CLASS_HWMON,      // I2C_CLASS_SPD
	.algo = myalgo
}


static int __init driver_init(void){
	int ret;
	ret = i2c_add_adapter(&adapter);

	if(ret < 0){
		pr_info("Failed to add adpater");   
		return FAILURE;
	}
	return SUCCESS;
}

static void __exit driver_exit(){
	i2c_del_adapter(&adapter);
	pr_info("removed adapter\n");
}

module_init(driver_init);  
module_exit(driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ziyad");
MODULE_DESCRIPTION("BUS Driver for MPu6050");
MODULE_VERSION("0.1");      

