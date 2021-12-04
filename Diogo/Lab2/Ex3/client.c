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

    //Verificar se já temos um PIPE/FIFO criado, se sim, abrir esse canal
	while((fd = open("/tmp/fifo_teste", O_WRONLY)) == -1){
      //Se não, temos que criar   
	  if(mkfifo("/tmp/fifo_teste", 0666)!=0){
			printf("Problem creating the fifo\n");
			exit(-1);
	  }else{
		  printf("Fifo created\n");
	  }
	}
	printf("Fifo just opened for writing\n");

    //Escrita do cliente, enquanto o servidor estiver à escuta para leitura 
	while(1){
        printf("Write f1 or f2: ");
		if (fgets(str, BUFFER_SIZE , stdin) != NULL){
            str[strlen(str)-1]='\0'; //MUITA ATENÇÂO 
		    write(fd, str, BUFFER_SIZE);
        }
	}

    return 0;
}
