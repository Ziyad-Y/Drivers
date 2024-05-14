
#include <linux/kernel.h>
#include <linux/init.h>  
#include <linux/module.h> 
#include <linux/moduleparam.h>  
#include <linux/stat.h>
#include <linux/types.h>
#include <linux/printk.h>

MODULE_LICENSE("GPL");  

static void __exit end_mod(void){
	pr_info("CLOSING MODULE\n");
}    


module_exit(end_mod);