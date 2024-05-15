/*
*Implement the Xorshiro128++ algorithm
*see https://prng.di.unimi.it/ PRNG algorithm
*/

#ifdef XORSHIRO128_PP 
#define XORSHIRO128_PP   

#include <linux/types.h>   
typedef int __bool;  
#define True 1   
#define False 0



struct xorshiro
{
	u64 SEED;
	u64 s[2];   
	__bool jump;
};

static void generator_init(struct xorshiro* ,u64, __bool);  
static u64 next(struct xorshiro *);   
static void jump(struct xorshiro *);   
#endif   


