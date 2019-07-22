//Implement a C function called ex3 such that it achieves 
//the same functionality as the machine code of objs/ex3_sol.o
//Note: you need to figure out ex3()'s function signature yourself; 
//the signature is not void ex3()
  
#include <assert.h>

int
ex3(char* c, char* d, int a) {
	int b;
	for (b = 0; b < a; b++){
		char e = c[b];
		c[b] = d[b];
		d[b] = e;	
	}
	return b;
}
