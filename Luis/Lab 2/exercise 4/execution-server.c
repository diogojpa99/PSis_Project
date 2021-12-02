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
    // Server reads from /tmp/fifo-execution and writes to /tmp/fifo-return.
    int fd_write, fd_read;

    // fd_read -- /tmp/fifo-execution (read-only)
    while((fd_read = open("/tmp/fifo-execution", O_RDONLY))==-1){
        if(mkfifo("/tmp/fifo-execution", 0666)!=0){
            printf("FIFO read file not created.\n");
            exit(-1);
        }
    }
    printf("FIFO read file opened successfully.\n");
    
    // fd_write -- /tmp/fifo-return (write-only)
    while((fd_write = open("/tmp/fifo-return", O_WRONLY))==-1){
        if(mkfifo("/tmp/fifo-return", 0666)!=0){
            printf("FIFO write file not created\n");
            exit(-1);
        }
    }
    printf("FIFO write file opened successfully");

    int n, value, (*fcn)();
    char str[BUFFER_SIZE];
    void *handle;
    

    while(1){
        n = read(fd_read, str, BUFFER_SIZE);
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
        write(fd_write, &value, sizeof(&value));
    }
    return 0;
}