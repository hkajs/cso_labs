
/***********************************************************
 File Name: bloom.c
 Description: implementation of bloom filter goes here 
 **********************************************************/

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "bloom.h"

/* Constants for bloom filter implementation */
const int H1PRIME = 4189793;
const int H2PRIME = 3296731;
const int BLOOM_HASH_NUM = 10;

/* The hash function used by the bloom filter */
int
hash_i(int i, /* which of the BLOOM_HASH_NUM hashes to use */ 
       long long x /* the element (a RK value) to be hashed */)
{
	return ((x % H1PRIME) + i*(x % H2PRIME) + 1 + i*i);
}

/* Initialize a bloom filter by allocating a character array that can pack bsz bits.
   Furthermore, clear all bits for the allocated character array. 
   Each char should contain 8 bits of the array.
   Hint:  use the malloc and memset library function 
	 Return value is the allocated character array.*/
bloom_filter
bloom_init(int bsz /* size of bitmap to allocate in bits*/ )
{
	bloom_filter f;
	assert((bsz % 8) == 0);
	f.bsz = bsz;
	int size = 0;
	char* arr;
	if (bsz % 8 == 0){
		arr = (char*) malloc(bsz/8);
		size = bsz/8;
	}else{
		arr = (char*) malloc(bsz/8 + 1);
		size = bsz/8 + 1;
	}
	f.buf = arr;
	memset(f.buf,0,size);
	return f;
}

/* Add elm into the given bloom filter 
 * We use a specific bit-ordering convention for the bloom filter implemention.
   Specifically, you are expected to use the character array in big-endian format. As an example,
   To set the 9-th bit of the bitmap to be "1", you should set the left-most
   bit of the second character in the character array to be "1".
*/
void
bloom_add(bloom_filter f,
	long long elm /* the element to be added (a RK hash value) */)
{	
	int hash;
	int index;
	int bit;
	for (int x = 0; x < BLOOM_HASH_NUM; x++){
		hash = hash_i(x,elm); //hashes the elm
		index = (hash % f.bsz)/8; //gives byte
		bit = 7-((hash % f.bsz)%8); //gives bit of byte (then changes to big endian with 7)
		f.buf[index] |= (1 << bit); //set bit to 1
	}
}

/* Query if elm is a probably in the given bloom filter */ 
int
bloom_query(bloom_filter f,
	long long elm /* the query element (a RK hash value) */ )
{	
	int hash;
	int index;
	int bit;
	for (int x = 0; x < BLOOM_HASH_NUM; x++){
		hash = hash_i(x,elm);
		index = (hash % f.bsz)/8;
		bit = 7-((hash % f.bsz)%8);
		if (((f.buf[index] >> bit) & 1) == 0){
			return 0;
		}
	}
	return 1;
}

void 
bloom_free(bloom_filter *f)
{
	free(f->buf);
	f->buf = (char *)0;
       	f->bsz = 0;
}

/* print out the first count bits in the bloom filter, 
 * where count is the given function argument */
void
bloom_print(bloom_filter f,
            int count     /* number of bits to display*/ )
{
	assert(count % 8 == 0);
	for(int i=0; i< (f.bsz>>3) && i < (count>>3); i++) {
		printf("%02x ", (unsigned char)(f.buf[i]));
	}
	printf("\n");
	return;
}
