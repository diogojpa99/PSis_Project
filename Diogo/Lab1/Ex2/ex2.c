#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

    int i,j, counter=0;
    char *result_str, *str_ptr, *aux_ptr;

    for(i=0; i<argc; i++){
        j=0;
        while (argv[i][j] != '\0'){
            counter++;
            j++;
        }
    }

    if((result_str = (char*) calloc(counter, sizeof(char))) == NULL){   
        printf("Memory is full. Couldn't register request.\n");
	    return -1;
    } 

    aux_ptr = str_ptr = result_str;

    for (i=0; i<argc; i++){

        counter=0;
        j=0;
        
        while (argv[i][j] != '\0'){
            counter++;
            j++;
        }

        str_ptr=aux_ptr;
        
        for(j=0; j<counter; j++){
            str_ptr[j] = argv[i][j];
            aux_ptr = &str_ptr[j+1]; 
        }
    }

    printf("Result_str: %s\n", result_str);

    free(result_str);

    return 0;
}