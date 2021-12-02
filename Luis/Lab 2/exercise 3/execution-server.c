#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <dlfcn.h>

#define BUFFER_SIZE 100

int main(){
    int fd;

    while((fd = open("/tmp/fifo-execution", O_RDONLY))==-1){
        if(mkfifo("/tmp/fifo-execution", 0666)!=0){
            printf("Error: FIFO not created.\n");
            exit(-1);
        }
    }
    printf("FIFO opened succesfully.\n");

    int n, value, fcn_sel, (*fcn)();
    char str[BUFFER_SIZE];
    void *handle;
    

    while(1){
        n = read(fd, str, BUFFER_SIZE);
        if(n<=0){
            printf("Read error.\n");
            exit(-1);
        }
        printf("Received str: %s n=%d\n", str, n);

        if(strcmp(str, "f1") && strcmp(str, "f2"))
            continue;

        handle = dlopen("./funcs.so", RTLD_LAZY);
        fcn = dlsym(handle, str);

        value = (*fcn)();
        printf("value = %d\n", value);
    }
    return 0;
}