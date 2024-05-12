#include <linux/kernel.h>
#include <linux/init.h>  
#include <linux/module.h> 
#include <linux/moduleparam.h>  
#include <linux/stat.h>
#include <linux/types.h>
#include <linux/printk.h>
//MODULE LICENSE   
MODULE_LICENSE("GPL");   
MODULE_AUTHOR("ME");      
MODULE_DESCRIPTION("My first module");  
MODULE_VERSION("0.1");

static  int myage = 100;   
static char * myname = "Random";
static int arr[3]={0,1,2};   
static int argc = 0;
//MODULE PARAMS
//--------- Single Types -----------------//
module_param(myage, int,S_IRUSR| S_IWUSR | S_IWGRP  | S_IRGRP);   
MODULE_PARM_DESC(myage, "Age")     
module_param(myname , charp , 0000);
MODULE_PARM_DESC(myname , "Name");
//-------- Arrays -------------------------//
module_param_array(arr, int,&argc, 0000);   
MODULE_PARM_DESC(arr, "INT ARRAY");


static int __init start_mod(void){
	int i;	
	pr_info("LOADING MODULE .... \n");
	pr_info("NAME : %s\n", myname);
	pr_info("I am %d years old \n", myage);      
	for(i =0 ; i < ARRAY_SIZE(arr); i++){
		pr_info( "myintArray[%d] = %d\n", i, arr[i]);;
	}
	pr_info("Got %d args in array", argc);
	return 0;  

}

static void __exit end_mod(void){
	printk(KERN_INFO "Exiting first module\n");
}    


module_init(start_mod);  
module_exit(end_mod);