#include <assert.h>

// Swap the values of the integer variables pointed to by p1 and
// p2.  After the function returns, the variable pointed to by p1
// should have the value once pointed to by p2, and vice-versa.
//
// You may use a temporary variable or do an XOR swap.
void
swap(int *p1, int *p2)
{
	assert(p1 != 0);
	assert(p2 != 0);
	
	*p1 = *p1 ^ *p2;
	*p2 = *p2 ^ *p1;
	*p1 = *p1 ^ *p2;
}

// bubble_sort implements the bubble sort algorithm.
// https://en.wikipedia.org/wiki/Bubble_sort
// It is given an integer array "arr" of "n" elements. 
// Upon return, the integer array should contain the sorted numbers
// in increasing order. 
// You should use the previous swap function when implementing bubble_sort
void
bubble_sort(int *arr, int n)
{	n -= 1;
	for (int x = 0; x < n; x++){
		for(int y = 0; y < n; y++){
			if (arr[y+1] < arr[y])
				swap(&arr[y+1],&arr[y]);
		}
	}
	return;
        assert(0);
}
