#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

    int i=0,j, len=0;
    char *result_str, *str_ptr;


    if((result_str = (char*) calloc(1024, sizeof(char))) == NULL){   
        printf("Memory is full. Couldn't register request.\n");
	    return -1;
    } 

    //strcpy(result_str,"\0");

    str_ptr = result_str;

    for (i=0; i<argc; i++){

        j=0;
        while (argv[i][j] != '\0'){
            
            j++;
        }

        for(j=0; j<len; j++){
            str_ptr[j] = argv[i][j];
            str_ptr += 1; // Devido ao '\0'
        }
    }

    printf("Result_str: %s\n", result_str);

    free(result_str);

    return 0;
}