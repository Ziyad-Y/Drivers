#include "rand64.h"  

MODULE_LICENSE("GPL");   
MODULE_AUTHOR("ME");      
MODULE_DESCRIPTION("Random 64-bit Number Stream");  
MODULE_VERSION("0.1");
//----------- Xorshiro128++ ---------------------------//
static inline u64 rotl(const u64, int k){
	return (x << k) | (x >> (64-k));


static void generator_init(struct xorshiro * xsh, u64 SEED, __bool jump){
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


void jump(struct xorshiro * xsh) {
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

static struct xorshiro x;   
generator_init(&x, SEED,jump);

//----------- End Xorshiro ----------------------//
static atomic_t is_open = ATOMIC_INIT(UNUSED);
static int major;   




static int device_open(struct inode * inode, struct file * file){
	if(atomic_cmpxchg(&is_open, UNUSED, OPENED ))
		return -EBUSY;    
	pr_info("OPENED CALLED\n");
	return SUCCESS;
}

static int device_release(struct inode * inode , struct file * file)
{
	atomic_set(&already_open, CDEV_NOT_USED);   
	module_put(THIS_MODULE);
	return SUCCESS;
}

static ssize_t device_read(struct file * file , char __user * buffer, size_t ,loff_t * offset){
	int copy;
	u64 random = next(&x);
	if(x->jump == True)
		jump(&x);
	char random_str [20];   
	sprintf(random_str, "%llu\n", random);  
	size_t length =  strnlen(random_str, 20); 
	copy = copy_to_user(buffer, random_str, length);
	if(copy !=0)
		return -EFAULT;    
	return length;
}


static struct file_operations devfops = {
	.read = device_read,      
	.open = device_open,   
	.release = device_release
};    

static int SEED = 1000;
static int jump = 0;
module_param(SEED, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);   
MODULE_PARM_DESC(SEED, "Random seed");
module_param(jump, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);   
MODULE_PARM_DESC(jump, "Random seed");



