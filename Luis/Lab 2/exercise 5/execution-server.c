#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <dlfcn.h>

#include "data_struct.h"

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
    message_type message;
    

    while(1){
        n = read(fd_read, &message, sizeof(message_type*));
        if(n<=0){
            printf("Read error.\n");
            exit(-1);
        }
        printf("Received:\n\tf_name: %s\n\tfunct_type=%d\n\targ=%d\n", message.f_name, message.funct_type, message.arg);

        if(strcmp(message.f_name, "f1") && strcmp(message.f_name, "f2") && strcmp(message.f_name, "f3"))
            continue;

        handle = dlopen("./funcs-ex5.so", RTLD_LAZY);
        fcn = dlsym(handle, message.f_name);
        if(message.funct_type==1){
            printf("funct_type=1 - int argument = %d.\n", message.arg);
            value = (*fcn)(message.arg);
        }
        else
            value = (*fcn)();
        write(fd_write, &value, sizeof(&value));
    }
    return 0;
}