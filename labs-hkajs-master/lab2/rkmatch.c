/*
	 ./rkmatch snippet_size query_doc doc1 [doc2...]

*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <ctype.h>

#include "bloom.h"

enum algotype { EXACT=0, SIMPLE, RK, RKBATCH};

/* a large prime for RK hash (BIG_PRIME*256 does not overflow)*/
long long BIG_PRIME = 5003943032159437; 


/* constants used for printing debug information */
const int PRINT_RK_HASH = 5;
const int PRINT_BLOOM_BITS = 160;


/* calculate modulo addition, i.e. (a+b) % BIG_PRIME */
long long
madd(long long a, long long b)
{
	assert(a >= 0 && a < BIG_PRIME && b >= 0 && b < BIG_PRIME);
	return ((a+b)>BIG_PRIME?(a+b-BIG_PRIME):(a+b));
}

/* calculate modulo substraction, i.e. (a-b) % BIG_PRIME*/
long long
mdel(long long a, long long b)
{
	assert(a >= 0 && a < BIG_PRIME && b >= 0 && b < BIG_PRIME);
	return ((a>b)?(a-b):(a+BIG_PRIME-b));
}

/* calculate modulo multiplication, i.e. (a*b) % BIG_PRIME */
long long
mmul(long long a, long long b)
{
	assert(a >= 0 && a < BIG_PRIME && b >= 0 && b < BIG_PRIME);
	/* either operand must be no larger than 256, otherwise, there is danger of overflow*/
	assert(a <= 256 || b <= 256); 
	return ((a*b) % BIG_PRIME);
}

/* read the entire content of the file 'fname' into a 
	 character array allocated by this procedure.
	 Upon return, *doc contains the address of the character array
	 *doc_len contains the length of the array
	 */
void
read_file(const char *fname, unsigned char **doc, int *doc_len) 
{

	int fd = open(fname, O_RDONLY);
	if (fd < 0) {
		perror("read_file: open ");
		exit(1);
	}

	struct stat st;
	if (fstat(fd, &st) != 0) {
		perror("read_file: fstat ");
		exit(1);
	}

	*doc = (unsigned char *)malloc(st.st_size);
	if (!(*doc)) {
		fprintf(stderr, " failed to allocate %d bytes. No memory\n", (int)st.st_size);
		exit(1);
	}

	int n = read(fd, *doc, st.st_size);
	if (n < 0) {
		perror("read_file: read ");
		exit(1);
	}else if (n != st.st_size) {
		fprintf(stderr,"read_file: short read!\n");
		exit(1);
	}
	
	close(fd);
	*doc_len = n;
}

/* The normalize procedure normalizes a character array of size len 
   according to the following rules:
	 1) turn all upper case letters into lower case ones
	 2) turn any white-space character into a space character and, 
	    shrink any n>1 consecutive whitespace characters to exactly 1 whitespace

	 When the procedure returns, the character array buf contains the newly 
     normalized string and the return value is the new length of the normalized string.

     hint: you may want to use C library function isupper, isspace, tolower
     To find out how to use isupper function, type "man isupper" at the terminal
*/
int
normalize(unsigned char *buf,	/* The character array contains the string to be normalized*/
	int len	    /* the size of the original character array */)
{	// If the char array is empty return 0.
	/*
	if (len == 1){
		if (!(isspace(*(buf))))
			*(buf) = tolower(*(buf));
		}else{
			*(buf) = ' ';
		}
		return 1;
	}
	//coding with headache 
	//First I strip front whitespace and move it to end.
	int x = 0;
	for (x; x < len; x++){
                if (!(isspace(buf[x]))){
                        buf[x] = tolower(buf[x]);
                }else{
                        buf[x] = ' ';
                }
        }
	int y = 0;
	int z = 0;
	while (x < len && buf[x] == ' ')
		x++;
	while (y >= 0 && buf[y] == ' ')
		y--;
	while (z < len && x <= y){
		buf[z] = buf[x];
		x++;
		z++;
	}	
	while (z < len){
		buf[z] = ' ';
		z++;
	}

	x = 0;
	int check = 0;
	while (x < len && check < len){
		if (x > 0 && x < len && buf[x] == ' ' && buf[x-1] == ' '){
			int shift = x;
			while (shift+1 < len){
				buf[shift] = buf[shift+1];
				shift++;
			}
			check += 1;	
		}else{
			x++;
			check++;
		}
	}
	
	// Find the first ' ' after non-whitespace.
	
	int index = len-1;
	while (index >= 0){
		if (buf[index] == ' ')
			index--;
		else
			break;		
	}

	// Set that index to '\0'.

	if (index+1 < len && buf[index+1] == ' ')
		buf[index+1] = '\0';

	return index+1;
       */
	//init vars.
	int x = 0;
	int y = 0;
	//gets the first non whitespace char.
	while (isspace(buf[x]))
		x += 1;
	while (x<len){
		//if the prior index is a whitespace char we set it to ' '.
		if(isspace(buf[x])){
			if (!isspace(buf[y-1])){
				buf[y++] = ' ';
			}
		}
		else{
		//converts all non whitespace to lowercase.
                        buf[y++] = tolower(buf[x]);
        	}
		x += 1;
	}
	//checks whether or not the index before the final y value is or is not a space.
	if (isspace(buf[y-1]))
		y -= 1;
	buf[y] = '\0';
	return y;

	//ive spent at least 7 hours everyday for the past 1.5 weeks looking
	//for bugs. I have had no time to actually sleep (2 midterms this week)
	//and try to recover from the flu. This normalization question
	//has made me frustrated to the point that
	//this specific function has really made me dislike c's
	//lack of safety features.
          
}


int
exact_match(const unsigned char *qs, int m, 
        const unsigned char *ts, int n)
{	
	//Very simple to see if 2 char arrays are equal.
	int x;	
	if (m != n){
		return 0;
	}else{
		for (x = 0; x < m; x++){
			if (*(qs+x) != *(ts+x))
				return 0;
		}
	}
   	return 1;
	
}

/* check if a query string ps (of length k) appears 
	 in ts (of length n) as a substring 
	 If so, return 1. Else return 0
	 You may want to use the library function strncmp
	 */
int
simple_substr_match(const unsigned char *ps,	/* the query string */
	int k,/* the length of the query string */
	const unsigned char *ts,/* the document string (Y) */ 
	int n/* the length of the document Y */)
{
	//What i am doing here is checking each char against eachother k times per loop of n.
	int x;
	int y;
	for (x = 0; x < n; x++){
		int count = 0;
		for (y = 0; y < k; y++){
			if (x + k > n || ts[x+count] != ps[y]){
				break;
			}else if (count == k - 1){
				return 1;
			}else{
				count += 1;
			}
		}
	}
	return 0;
}

/* initialize the Rabin-karp hash computation by calculating 
 * and returning the hash over a charbuf of k characters.
 * i.e. return value should be 256^(k-1)*charbuf[0] + 256^(k-2)*charbuf[1] + ... + charbuf[k-1]
 * it also returns the value 256^k in the long long variable pointed to by h
 * Note: We use the "long long" type to represent an RK hash.
 * Note that the above *,-,+ operator must be done as modular arithematic.(madd, mdel, mmul)
 */
long long
rkhash_init(const unsigned char *charbuf, int k, long long *h)
{	
	//Loops over ther char array and generates a rolling hash val
	//Also sets h to 256^k
	int x;
	long long sum = 0;
	*h = 1;
	for (x = 0; x < k; x++){
		*h = mmul(*h,(long long)256);
	}
	for (x = 0; x < k; x++){
		sum = madd(mmul(sum,(long long)256),(long long)charbuf[x]);
	}
	return sum;
}

/* Given the rabin-karp hash value (curr_hash) over substring Y[i],Y[i+1],...,Y[i+k-1]
 * calculate the hash value over Y[i+1],Y[i+2],...,Y[i+k] 
 * = curr_hash * 256 - prev * h + next
 * Note that the above *,-,+ operator must be done as modular arithematic.(madd, mdel, mmul)
 * The call of rkhash_next must invoke the function with h = 256^k and prev = Y[i] and next = Y[i+k]
 */
long long 
rkhash_next(long long curr_hash, long long h, unsigned char prev, unsigned char next)
{
	//Order of operations means that mul happens before del and add.
	return madd(mdel(mmul(curr_hash,(long long)256),mmul(prev,h)),next);
}

/* Check if a query string ps (of length k) appears 
   in ts (of length n) as a substring using the rabin-karp algorithm
   If so, return 1. Else return 0

   Hint: Complete and use rkhash_init and rkhash_next functions

   In addition, print the hash value of ps (on one line)
   as well as the first 'PRINT_RK_HASH' hash values of ts (on another line)
   Example:
	 $ ./rkmatch -t 2 -k 20 X Y
	 4537305142160169
	 1137948454218999 2816897116259975 4720517820514748 4092864945588237 3539905993503426 
	 2021356707496909
	 1137948454218999 2816897116259975 4720517820514748 4092864945588237 3539905993503426 
	 0 chunks matched (out of 2), percentage: 0.00 
   Hint: Use printf("%lld", x) to print a long long type.
   
   Important: Since we want to test program's correctness, do NOT return 1 immediately after 
   you found the query string is matched. Please print the mentioned hash value even the result
   is matched and return the 1 or 0 at the end of the function.
   */
int
rabin_karp_match(const unsigned char *ps,	/* the query string */
	int k,/* the length of the query string */
	const unsigned char *ts,/* the document string (Y) */ 
	int n/* the length of the document Y */)
{	
	//Initialization
	long long hashq = 1;
	long long hashd = 1;
	long long *hash_query = &hashq;
	long long *hash_doc = &hashd;
	long long dhash = rkhash_init(ts,k,hash_doc); 
	long long qhash = rkhash_init(ps,k,hash_query);
	int check = 0;
	if (dhash == qhash)
		check = 1;
	int x = 0;
	printf("%llu\n",qhash);
	for (x; x < n - k + 1; x++){
		//Get next substring hash and check against query
		long long hashcheck = rkhash_next(dhash,*hash_doc,ts[x-k],ts[x+k]);
		if (hashcheck == qhash)
			check = 1;
		if (x < PRINT_RK_HASH)
			printf("%llu ",hashcheck);
	}
	printf("\n");
	return check;
}


/* Allocate a bitmap of bsz bits for the bloom filter (using bloom_init)
   and insert all m/k RK hashes of qs into the bloom filter (using bloom_add).  
   Compute each of the n-k+1 RK hashes of ts and check if it's in the filter (using bloom_query). 
   This function returns the total number of matched chunks. 
   For testing purpose, you should print out the first PRINT_BLOOM_BITS bits 
   of bloom bitmap in hex after inserting all m/k chunks from qs.
   Important: accoring to our counting rule, each segmented X(size k) should only be counted as once,
              so you must maintain a data structure to check if that segmented X(size k) has been counted
              already.
 */
int
rabin_karp_batchmatch(int bsz, /* size of bitmap (in bits) to be used */
    int k,/* chunk length to be matched */
    const unsigned char *qs, /* query docoument (X)*/
    int m,/* query document length */ 
    const unsigned char *ts, /* to-be-matched document (Y) */
    int n /* to-be-matched document length*/)
{
	int x;
	bloom_filter bmap = bloom_init(bsz);	
	for (x = 0; x < m; x++){
		bloom_add(bmap,qs[x]);
	}
	bloom_print(bmap,bsz);
	
	//this is the hardest assignment on anything ive ever done in my life.	
    /* Your code here */
    /* step 1: allocate bloom filter(bloom_init) and insert query string(segment document X by size k)
       one by one by using bloom_add */
    /* step 2: print bloom filter's result by using bloom_print */
    /* step 3: initiize document Y's first hash value */
    /* step 4: compare segmented Y's and segmented X's hash values by using bloom_query, 
               if matched, compare each segmented X(size k) to present segmented Y. And
               don't forget the Important reminder */
    /* step 5: return matched number */
               
    return 0;
}

