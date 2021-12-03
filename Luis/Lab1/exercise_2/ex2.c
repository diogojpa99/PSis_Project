#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    char *return_str;   // Output string
    int size=1,         // Size of array return_str
        position=0, k,  // Used to access array positions
        *argSizes;      // Array containing the sizes the strings provided by the user
    
    if(argc==1){
        printf("No strings provided.\n");
        return 0;
    }

    // Create and set argSizes
    if((argSizes=malloc((argc-1)*sizeof(int)))==NULL){
        printf("Error: Not enough memory available.\n");
        exit(1);
    }
    for(int i=0; i<argc-1; i++) argSizes[i] = 0;
    for(int i=1;i<argc;i++){
        k=0;
        while(argv[i][k] != '\0'){
            argSizes[i-1]++;
            k++;
        }
        size=size + argSizes[i-1];
    }

    if((return_str=malloc(size))==NULL){
        printf("Error: Not enough memory available.\n");
        exit(1);
    }
    // Copy the characters in the input strings into return_str one-by-one
    for(int i=1; i<argc; i++){
        for(int j=0;j<argSizes[i-1];j++){
            return_str[position+j]=argv[i][j];
        }
        position = position+argSizes[i-1];
    }
    return_str[position]='\0';

    // Print return_str
    puts(return_str);

    if(return_str!=NULL) free(return_str);
    if(argSizes!=NULL) free(argSizes);

    return 0;
}