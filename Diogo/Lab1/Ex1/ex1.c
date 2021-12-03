#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){

    int i, counter=0;
    char *result_str;

    //Tamanho de string
    for (i=0; i<argc; i++)
        counter += strlen(argv[i]);

    //Alocar memória para string (Atenção ao +1 - Devido ao '\0' -> String)
    if((result_str = (char*) calloc(counter + 1, sizeof(char))) == NULL){   
        printf("Memory is full. Couldn't register request.\n");
	    return -1;
    } 

    //Concatenação
    for (i=0; i<argc; i++)
        strcat(result_str,argv[i]);

    printf("Result_str: %s\n", result_str);

    free(result_str);

    return 0;
}