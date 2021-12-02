#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "data_struct.h"

#define BUFFER_SIZE 100

int main(){
	// Client reads from /tmp/fifo-return and writes to /tmp/fifo-execution
    int fd_write, fd_read, n, a, value;
    char str[BUFFER_SIZE];
	message_type message;

	// fd_write -- /tmp/fifo-execution (write-only)
    while((fd_write = open("/tmp/fifo-execution", O_WRONLY))== -1){
	  if(mkfifo("/tmp/fifo-execution", 0666)!=0){
			printf("FIFO write file not created.\n");
			exit(-1);
	  }else{
		  printf("FIFO write file opened successfully.\n");
	  }
	}

	//fd_read -- /tmp/fifo-return (read-only)
	while((fd_read = open("/tmp/fifo-return", O_RDONLY)) == -1){
		if(mkfifo("/tmp/fifo-return", 0666)!=0){
			printf("FIFO read file not created.\n");
			exit(-1);
		}
	}
	printf("FIFO read file opened succesfully.\n");

  	while(1){
		printf("Write a function name: ");
		fgets(str, BUFFER_SIZE, stdin);
        str[strlen(str)-1]='\0';
		strcpy(message.f_name, str);
		// check if function has arguments and request them.
		if(strcmp(message.f_name, "f3")==0){
			printf("\tProvide an integer argument: ");
			fgets(str, BUFFER_SIZE, stdin);
			if((sscanf(str, "%d", &message.arg))!=1) continue;
			message.funct_type=1; // type=1 indicates the function receives an integer as argument.
		}
		else
			message.funct_type=0; // type=0 indicates the function takes no arguments.		
		write(fd_write, &message, sizeof(message_type*));
		printf("Sent:\n\tf_name: %s\n\tfunct_type=%d\n\targ=%d\n", message.f_name, message.funct_type, message.arg);
		n = read(fd_read, &value, sizeof(&value));
		if(n<=0){
			printf("Read error.\n");
			exit(-1);
		}
		printf(" value=%d.\n", value);
	}
}