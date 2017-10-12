#include <stdio.h>
#include <stdlib.h>

/*
 * Compute an xor-based hash of the data provided on STDIN. The result
 * should be placed in the array of length block_size pointed to by
 * hash_val.
 */
char* hash(FILE *f) {
    int i;
    char *hash_val;
    hash_val = malloc(sizeof(char)*8);
    for(i = 0; i < 8; i++){
    	hash_val[i] = '\0';
    }
    int j = 0;
    int c;
    if (f) {
        while ((c = getc(f)) != EOF){
            hash_val[j] = (hash_val[j] ^ c);
            j++;
	    if(j >= 7){
		j = 0;
	    }
        }
    fclose(f);
    }
    
    return hash_val;
}

/* 
 * Print the message "Hashes match" if the two hashes of size 
 * block_size contain the same value. Print "Hash mismatch" otherwise.
 */
void check_hash(const char *hash1, const char *hash2, long block_size) {
    int i;
    int count_mismatch = 0;
    int converter = 0;
    for(i = 0; i < block_size; i++){
	converter = hash2[i] - '0';
	if((hash1[i] != converter)){
	    count_mismatch++;
	}
    }	
    if(count_mismatch > 0){
	printf("\nHash mismatch\n");
    }
    else{
    	printf("\nHash match\n");
    }
}
