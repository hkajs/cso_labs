#include <stdio.h>
#include <string.h>
#include <assert.h>

//extract the 8-bit exponent field of single precision floating point number f 
//and return it as an unsigned byte
unsigned char
get_exponent_field(float f) 
{	
	//cast then shift mantissa off.
	unsigned int *ptr = (unsigned int*)&f;
	*ptr = *ptr >> 23;
	return *(unsigned char*) ptr;

}

//clear the most significant b bits of unsigned integer number
//return the resulting value
//As an example, suppose unsigned int number = 0xffff0000; int b= 15
//then the correct return value is 65536
//if b = 30, then the return value is 0
//if b = 40, the return value should also be zero
unsigned int
clear_msb(unsigned int number, int b)
{
	number = number << b;
	number = number >> b; //place values need to be reset
	return number;
	assert(0);
}

//given an array of bytes whose length is array_size (bytes), treat it as a bitmap (i.e. an array of bits),
//and return the bit value at index i (from the left) of the bitmap.
//As an example, suppose char array[3] = {0x00, 0x1f, 0x12}, the corresponding
//bitmap is 0x001f12, thus,
//the bit at index 0 is bitmap[0] = 0
//the bit at index 1 is bitmap[1] = 0
//...
//the bit at index 11 is bitmap[11] = 1
//the bit at index 12 is bitmap[12] = 1
//...
//the bit at index 16 is bitmap[16] = 0
//...
unsigned char
bit_at_index(unsigned char *array, int array_size, int i)
{	
	/* non-bitwise attempt
	int len = array_size * 8;
	int bits[len];
	int incr = 0;
	for (int x = 0; x < array_size; x++){
		int holder[8];
		int z = 0;
		for (int k = 0; k < 8; ++k){
			holder[z] =  (int) ((array[x] >> k) & 1);
			z += 1;
		}
		for (int y = 0; y < 8; y++){
			bits[y+incr] = holder[y];		
		}
		incr += 8;
	}
	return (unsigned char) bits[i];
	*/
	
	//byte = 8 bits so break element down into bits
	int lo = i / 8;
	int div = i % 8;
	unsigned char c = array[lo];
	c = c << div; //shit left by div
	c = c >> 7; //shift right to correct element (8-7 = 1)
	if (c&1 == 1)
		return 1;
	else
		return 0;
	assert(0);
}
