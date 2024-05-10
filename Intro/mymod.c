#include <linux/kernel.h>
#include <linux/init.h>  
#include <linux/module.h> 
#include <linux/moduleparam.h>  
#include <linux/stat.h>
#include <linux/types.h>

//MODULE LICENSE   
MODULE_LICENSE("GPL");   
MODULE_AUTHOR("ME");      
MODULE_DESCRIPTION("My first module");  
MODULE_VERSION("0.1");

static  int myage = 100;   
static char myname[20] = "Random";
static int arr[3]={0,1,2};   
static int argc=0;
//MODULE PARAMS
//--------- Single Types -----------------//
module_param(myage, int,S_IRUSR| S_IWUSR | S_IWGRP  | S_IRGRP);   
MODULE_PARM_DESC(myage, "Age")     
module_param(myname , charp , S_IRUSR| S_IRGRP | S_IROTH );
MODULE_PARM_DESC(myname , "Name");
//-------- Arrays -------------------------//
module_param_array(arr, int, &argc, S_IRUSR|S_IRGRP | S_IROTH);   
MODULE_PARM_DESC(arr, "INT ARRAY");

static int data __initdata = 23;

static int __init start_mod(void){
	int i;	
	printk(KERN_INFO "Loading my first module ..... \n"); 
	printk(KERN_INFO "My Module %d \n ", data);
	printk(KERN_INFO "Name is : %s\n", myname);
	printk(KERN_INFO "I am %d years old \n", myage);      
	for(i =0 ; i < 3 ; i++){
		printk(KERN_INFO "myintArray[%d] = %d\n", i, arr[i]);;
	}
	return 0;  

}

static void __exit end_mod(void){
	printk(KERN_INFO "Exiting first module\n");
}    


module_init(start_mod);  
module_exit(end_mod);