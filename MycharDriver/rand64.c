#include "rand64.h"  

MODULE_LICENSE("GPL");   
MODULE_AUTHOR("ME");      
MODULE_DESCRIPTION("Random 64-bit Number Stream");  
MODULE_VERSION("0.1");
//----------- Xorshiro128++ ---------------------------//
static inline u64 rotl(const u64 x, int k){
	return (x << k) | (x >> (64-k));
}

static void generator_init(struct xorshiro * xsh, u64 SEED, int jump){
	xsh->s[0] = SEED;    
	xsh->s[1]= 0x02AC45BBF9ULL;   
	xsh->jump = jump; 
}

static u64 next(struct xorshiro * xsh){
	u64 s0 =xsh->s[0];   
	u64 s1 = xsh->s[1];   
	const u64 result = rotl(s0+s1, 17) + s0;
	s1 ^= s0;
	xsh->s[0] = rotl(s0, 49) ^ s1 ^ (s1 << 21); 
	xsh->s[1] = rotl(s1, 28); 
	return result;
}


static void jump(struct xorshiro * xsh) {
	static const u64 JUMP[] = { 0x2bd7a6a6e99c2ddc, 0x0992ccaf6a6fca05 };

	u64 s0 = 0;
	u64 s1 = 0;
	for(int i = 0; i < sizeof JUMP / sizeof *JUMP; i++)
		for(int b = 0; b < 64; b++) {
			if (JUMP[i] & ( (u64) 1U<< b)) {
				s0 ^= xsh->s[0];
				s1 ^= xsh->s[1];
			}
			next(xsh);
		}

	xsh->s[0] = s0;
	xsh->s[1] = s1;
}



//----------- End Xorshiro ----------------------//

/*-------------------- Begin module--------------*/
static atomic_t is_open = ATOMIC_INIT(UNUSED);
static int major;   
static struct class * cls;
static int SEED = 1000;
static int j = 0;
static struct xorshiro x;

module_param(SEED, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);   
MODULE_PARM_DESC(SEED, "Random seed");
module_param(j, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);   
MODULE_PARM_DESC(jump, "Jump");   




static int device_open(struct inode * inode, struct file * file){
	if(atomic_cmpxchg(&is_open, UNUSED, OPENED ))
		return -EBUSY;    
	pr_info("OPENED CALLED\n");
	return SUCCESS;
}

static int device_release(struct inode * inode , struct file * file)
{
	atomic_set(&is_open, UNUSED);   
	module_put(THIS_MODULE);
	return SUCCESS;
}

static ssize_t device_read(struct file * file , char __user * buffer, size_t length ,loff_t * offset){
	int copy;
	char random_str [200];   
	size_t l;
	u64 random;   

	random = next(&x);
	if(x.jump == 1)
		jump(&x);
	
	sprintf(random_str, "%llu\n", random);  
	l =  strnlen(random_str, 20); 
	copy = copy_to_user(buffer, random_str, l);
	if(copy !=0)
		return -EFAULT;    

	return l;
}


static struct file_operations fops = {
	.read = device_read,      
	.open = device_open,   
	.release = device_release
};    

static int __init rand64init(void){
	generator_init(&x, SEED,j);
	major  = register_chrdev(0, DEVICE_NAME,&fops );   
	
	if(major < 0){
		pr_alert("REGISTERING RAND64 %d\n", major);  
		return major;
	}

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(6,4,0)
		cls = class_create(DEVICE_NAME);    

	#else  
		cls = class_create(THIS_MODULE, DEVICE_NAME);    

	#endif 
	
	device_create(cls,NULL, MKDEV(major, 0), NULL, DEVICE_NAME);   
	return SUCCESS;
}

static void __exit rand64exit(void){
	device_destroy(cls,MKDEV(major,0));   
	class_destroy(cls);
	unregister_chrdev(major,DEVICE_NAME);
}

module_init(rand64init);   
module_exit(rand64exit);

