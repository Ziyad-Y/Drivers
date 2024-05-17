/*
*Implement the Xorshiro128++ algorithm
*see https://prng.di.unimi.it/ PRNG algorithm
*/

#ifdef XORSHIRO128_PP 
#define XORSHIRO128_PP   

#include <linux/types.h>   



struct xorshiro
{
	u64 SEED;
	u64 s[2];   
	int jump;
};

static void generator_init(struct xorshiro* ,u64, int);  
static u64 next(struct xorshiro *);   
static void jump(struct xorshiro *);   
#endif   


