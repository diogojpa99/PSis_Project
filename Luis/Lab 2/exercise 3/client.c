#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 100

int main(){
    int fd, n;
    char str[BUFFER_SIZE];

    while((fd = open("/tmp/fifo-execution", O_WRONLY))== -1){
	  if(mkfifo("/tmp/fifo-execution", 0666)!=0){
			printf("FIFO not created.\n");
			exit(-1);
	  }else{
		  printf("fifo created\n");
	  }
	}

  	while(1){
		printf("Write a function name: ");
		fgets(str, BUFFER_SIZE, stdin);
        str[strlen(str)-1]='\0';
		write(fd, str, BUFFER_SIZE);
    }
}