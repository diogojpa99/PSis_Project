#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

    int i,j, counter=0;
    char *result_str, *str_ptr;

    //Contagem do tamanho da result_str
    for(i=0; i<argc; i++){
        j=0;
        while (argv[i][j] != '\0'){
            counter++;
            j++;
        }
    }

    //Alocação de memória ( +1 é devido ao '\0' - Atanção -)
    if((result_str = (char*) calloc(counter+1, sizeof(char))) == NULL){ 
        printf("Memory is full. Couldn't register request.\n");
	    return -1;
    } 

    str_ptr = result_str;

    for (i=0; i<argc; i++){

        //Contagem do comprimento de cada string
        counter=0;
        j=0;
        while (argv[i][j] != '\0'){
            counter++;
            j++;
        }
        
        //Concatenação
        for(j=0; j<counter; j++)
            str_ptr[j] = argv[i][j];

        //Atualizar a posição do ponteiro de concateção 
        str_ptr = &str_ptr[j]; 
    }

    printf("Result_str: %s\n", result_str);

    free(result_str);

    return 0;
}