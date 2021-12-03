#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){
    char *return_str;
    int size=1;
    
    if(argc==1){
        printf("No strings provided.\n");
        return 0;
    }

    for(int i=1;i<argc;i++)
        size=size + strlen(argv[i]);

    if((return_str=malloc(size))==NULL){
        printf("Error: Not enough memory available.\n");
        exit(1);
    }else{
        strcpy(return_str, argv[1]);
        for(int i=2;i<argc;i++)
            strcat(return_str, argv[i]);

        printf("%s\n", return_str);

        free(return_str);
    }
    return 0;
}