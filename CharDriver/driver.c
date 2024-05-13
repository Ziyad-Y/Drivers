#include "chardev.h"
MODULE_DESCRIPTION("First Char Driver");   
MODULE_AUTHOR("ME");  
MODULE_VERSION("0.0.1");    
MODULE_LICENSE("GPL");    


module_init(chardev_init);     
module_exit(chardev_exit);