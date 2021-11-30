#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){

    int i;
    char *result_str;


    if((result_str = (char*) calloc(1024, sizeof(char))) == NULL){   
        printf("Memory is full. Couldn't register request.\n");
	    return -1;
    } 

    for (i=0; i<argc; i++)
        strcat(result_str,argv[i]);

    printf("Result_str: %s\n", result_str);

    free(result_str);

    return 0;
}