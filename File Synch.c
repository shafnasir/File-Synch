#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <libgen.h>

#include "ftree.h"
#include "hash.h"

#define MAX 40000

int process_counter;
int copy_ftree(const char *src, const char *dest){
  //  printf("Src: %s, Dest: %s\n", src, dest);
    int r;
    int statval;
    struct stat src_stat;
    struct stat dest_stat;    
    //printf("dest: %d, src: %d\n", lstat(dest, &dest_stat), lstat(src, &src_stat));
	
    if(lstat(dest, &dest_stat) < 0){
        perror("dest:DNE\n");
        exit(-1);
    }
    if(lstat(src, &src_stat) < 0){
        perror("src:DNE\n");
        exit(-1);
    }

    char *basename_src;
    char *basename_dest;
    basename_src = malloc(sizeof(char) * strlen(src));
    basename_dest = malloc(sizeof(char) * strlen(dest));
    strcpy(basename_src, src);
    strcpy(basename_dest, dest);
    basename_src = basename(basename_src);
    basename_dest = basename(basename_dest); 
 
    if((strcmp(src, ".") == 0) || (strcmp(src, "..") == 0)){
	ino_t src_ino = src_stat.st_ino;
        struct stat src_parent;
	strcat(basename_src, "/..");
	if(lstat(basename_src, &src_parent) < 0){
            perror("dest:DNE\n");
            exit(-1);
        }
	DIR *src_Dr;
//	printf("%s\n", basename_src); 
	src_Dr = opendir(basename_src);
	// printf("%s\n", basename_src);
	if(src_Dr == NULL){
	    perror("dir DNE");
	    exit(-1);
	}
        struct dirent *src_diren;
        while((src_diren = readdir(src_Dr))){
            if(src_diren->d_name[0] == '.'){
                continue;
            }
            if(src_diren->d_ino == src_ino){
                src = src_diren->d_name;
            }

         }
	 closedir(src_Dr);

    }     

    if((S_ISLNK(src_stat.st_mode) > 0) || (S_ISLNK(dest_stat.st_mode) > 0)){
	perror("src or dest is a symbolic link");
	exit(-1);
    }
    if(S_ISDIR(dest_stat.st_mode) < 0){
    	perror("Destination not a directory\n");
        exit(-1);
    }   

   if(((S_ISREG(src_stat.st_mode)) == 1) && ((S_ISREG(dest_stat.st_mode)) == 1)){

	if(strcmp(basename_src, basename_dest) == 0){
        
	    if(src_stat.st_size == dest_stat.st_size){
		FILE *src_File;
		FILE *dest_File;
		src_File = fopen(src, "r");
        	dest_File = fopen(dest, "r");
		if(src_File == NULL || dest_File == NULL){
		    perror("src or dest DNE");
		    exit(-1);
		}
		char *src_hash;
                char *dest_hash;
		src_hash = malloc(sizeof(char) * 8);
	        dest_hash = malloc(sizeof(char) * 8);
          	src_hash = hash(src_File);
		dest_hash = hash(dest_File);
		
             if((strncmp(src_hash, dest_hash, 8)) != 0){
		
		    FILE *dest_copy;
		    FILE *src_copy2;
		    src_copy2 = fopen(src, "r");
		    dest_copy = fopen(dest, "w");
		    if(src_copy2 == NULL || dest_copy == NULL){
                        perror("src or dest DNE");
			exit(-1);
                    }
                 char elements[MAX] = {0};
                 int character = 0;
                 while((character = fread(elements, sizeof(char), 200, src_copy2))){    
		     if(fwrite(elements, sizeof(char), character, dest_copy) != character){
	                perror("Error: items not read properly");                        	
		        exit(-1);
                     }

                 }
	         	
                if(chmod(dest, src_stat.st_mode) == -1){
                    perror("Error");
		    exit(-1);
                }  		    
		}

            }
	    else{
		FILE *src_File;
                FILE *dest_File;
                src_File = fopen(src, "r");
                dest_File = fopen(dest, "w");
            	if(src_File == NULL || dest_File == NULL){
                    perror("src or dest DNE");
		    exit(-1);
                }
                char elements[MAX] = {0};
		int character = 0;
               	while((character = fread(elements, sizeof(char), 200, src_File))){
		    if(fwrite(elements, sizeof(char), character, dest_File) != character){
	                perror("Error: items not read properly");
                        exit(-1);
		    }

                }
		if(chmod(dest, src_stat.st_mode) == -1){
		    perror("Error");		
		    exit(-1);
		}
	    }
	}
    }
   // printf("src st: %d, dest st: %d\n", S_ISDIR(src_stat.st_mode), S_ISDIR(dest_stat.st_mode));
    if((S_ISREG(src_stat.st_mode)) == 1 && (S_ISDIR(dest_stat.st_mode) == 1)){
        FILE *src_File;
	FILE *src_copy;

        DIR *src_Dire;

        src_Dire = opendir(dest);
        if(src_Dire == NULL){
            perror("diir DNE");
            exit(-1);
        }

        struct dirent *srce_dirent;

	while((srce_dirent = readdir(src_Dire))){
	     if(strcmp(src, srce_dirent->d_name) == 0){
		copy_ftree(src, srce_dirent->d_name);
		return 1;
	     }
	} 

       	char *file_copy;	
	file_copy = malloc((sizeof(char)*(strlen(dest) + strlen(src))) + 1);
	strcpy(file_copy, dest);
	strcat(file_copy, "/");
        char *base_src = malloc(sizeof(char)*(strlen(src)));
        char *copy_src;
        copy_src = malloc(sizeof(char)*(strlen(src)));
	strcpy(copy_src, src);
        base_src = basename(copy_src);
        strcat(file_copy, base_src);	
        src_File = fopen(src, "r");
	src_copy = fopen(file_copy, "w"); 
        if(src_copy == NULL|| src_File == NULL){
            perror("src or dest DNE");
	    exit(-1);
        }
        char elements[MAX] = {0};	
	int character = 0;
        while((character = fread(elements, sizeof(char), 200, src_File))){
	    if(fwrite(elements, sizeof(char), character, src_copy) != character){
		perror("Error: items not read properly");
		exit(-1);
	    }
	}
	if(chmod(file_copy, src_stat.st_mode) == -1){
            perror("Error");
	    exit(-1);
        }		
    }
  	
    else if((S_ISDIR(src_stat.st_mode)) == 1 && (S_ISDIR(dest_stat.st_mode)) == 1){
	DIR *src_Dir;
	src_Dir = opendir(src);

	if(src_Dir == NULL){
	    perror("dir DNE");
	    exit(-1);
	}
   
	struct dirent *src_dirent;

	char *dir_copy_name;
        dir_copy_name = malloc((sizeof(char)*(strlen(dest) + strlen(src))) + 1);
        strcpy(dir_copy_name, dest);
        strcat(dir_copy_name, "/");
	char *base_src = malloc(sizeof(char)*(strlen(src)));
	char *copy_src;
	copy_src = malloc(sizeof(char)*(strlen(src)));
	strcpy(copy_src, src);
	base_src = basename(copy_src);
        strcat(dir_copy_name, base_src);
	//printf("%s, %s\n", src, dest);
//	printf("before:%s\n",dir_copy_name);
	
	mkdir(dir_copy_name, src_stat.st_mode);	

	while((src_dirent = readdir(src_Dir))){
	    if(src_dirent->d_name[0] == '.'){
		continue;
	    }
	    
	    if(src_dirent -> d_type == DT_REG){
		char *f_copy;
                f_copy = malloc((sizeof(char)*(strlen(dest) + strlen(src))) + 1);
                strcpy(f_copy, src);
                strcat(f_copy, "/");
                strcat(f_copy, src_dirent->d_name);
	//	printf("File: src: %s, dest: %s\n",src_dirent->d_name, dir_copy_name);
		copy_ftree(f_copy, dir_copy_name);
	    }
		    
 	    else if(src_dirent->d_type == DT_DIR){   
	      	char *d_copy;
        	d_copy = malloc((sizeof(char)*(strlen(dest) + strlen(src))) + 1);
		//printf("DIR: src: %s, dest: %s\n",src_dirent->d_name, dir_copy_name);
	        strcpy(d_copy, src);
        	strcat(d_copy, "/");
        	strcat(d_copy, src_dirent->d_name);  	
	 	r = fork();
		
		if(r == 0){
		    
	//	    printf("%s, %s\n", basename_src, basename_dest);
	//	    printf("d: %s, dir: %s\n", d_copy, dir_copy_name);
		    copy_ftree(d_copy, dir_copy_name);
   	            process_counter += 1;
         	    exit(process_counter);
	
	        }
	
	    }
	
	}
	if(r != 0){
	    
	    wait(&statval); 
	    process_counter += WEXITSTATUS(statval);
	    closedir(src_Dir);
	    
	}
  }
if(process_counter == 127){
    return 1;
}
return process_counter+1;
   
}
