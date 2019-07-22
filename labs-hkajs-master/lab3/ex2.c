//Implement a C function called ex2 such that it achieves 
//the same functionality as the machine code of objs/ex2_sol.o
//Note: you need to figure out ex2()'s function signature yourself; 
//the signature is not void ex2()
  
#include <assert.h>

int
ex2(char* str) {
	int ret = 0;
	int iter = 0;
	while (str[iter] != '\0'){
		ret += (str[iter] == 'a');
		iter++;
	}
	return ret;
}
