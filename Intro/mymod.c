#include <linux/kernel.h>
#include <linux/init.h>  
#include <linux/module.h> 
  
//MODULE LICENSE   
MODULE_LICENSE("GPL");   

// AUTHOR  
MODULE_AUTHOR("ME");  

// MODULE DESCRIPTION     
MODULE_DESCRIPTION("My first module");  

// MODULE VERSION 
MODULE_VERSION("0.0.1");

static int __init start_mod(void){
	printk(KERN_INFO "Loading my first module ..... \n"); 
	printk(KERN_INFO "My Module\n");

	return 0;  

}

static void __exit end_mod(void){
	printk(KERN_INFO "Exiting first module\n");
}    


module_init(start_mod);  
module_exit(end_mod);