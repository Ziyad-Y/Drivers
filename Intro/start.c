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


//------------ declared types ---------------//   
static char *first_name = "Unknown First"; 
static char *last_name= "Unknown Last";  
static int age = 0;   
static int arr[3]={1,2,3};   
static int arr_count = 0;

//--------- Single Types -----------------//
module_param(age, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(age, "An integer"); 
module_param(first_name, charp, 0000);   
MODULE_PARM_DESC(first_name, "A character string");   
module_param(last_name, charp,0000);   
MODULE_PARM_DESC(last_name, "A character string");

//------- Array Types---------------------//
module_param_array(arr, int, &arr_count, 0000);   
MODULE_PARM_DESC(arr, "An array of integers");

static int __init start_mod(void){
	int i;	
	pr_info("LOADING MODULE ......\n");   
	pr_info("NAME : %s %s\n", first_name, last_name);   
	pr_info("AGE : %d\n", age);   
	for(i=0 ; i< ARRAY_SIZE(arr);i++)
		pr_info("arr[%d]= %d\n", i, arr[i]);
	pr_info("got %d values in array\n", arr_count);
	return 0;  

}

static void __exit end_mod(void){
	pr_info("CLOSING MODULE\n");
}    


module_init(start_mod);  
module_exit(end_mod);