#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 100


int main(){


	int fd_w, fd_r, f_return, n;
	char str[BUFFER_SIZE];

    //Verificar se já temos um PIPE/FIFO criado, se sim, abrir esse canal
	while((fd_w = open("/tmp/fifo_teste", O_WRONLY)) == -1){
      //Se não, temos que criar   
	  if(mkfifo("/tmp/fifo_teste", 0666)!=0){
			printf("Problem creating the fifo\n");
			exit(-1);
	  }else{
		  printf("Fifo created\n");
	  }
	}
	printf("Fifo_write just opened for writing\n");

	//fd_read -- /tmp/fifo-return (read-only)
	while((fd_r = open("/tmp/fifo_return", O_RDONLY)) == -1){
		if(mkfifo("/tmp/fifo_return", 0666)!=0){
			printf("FIFO read file not created.\n");
			exit(-1);
		}
	}
	printf("Fifo_read just opened successfully\n");

    //Escrita do cliente, enquanto o servidor estiver à escuta para leitura 
	while(1){

        printf("Write f1 or f2: ");
		if (fgets(str, BUFFER_SIZE , stdin) != NULL){
            str[strlen(str)-1]='\0'; //MUITA ATENÇÂO 
		    write(fd_w, str, BUFFER_SIZE);
        }

		//Se o utilizador não escrever o que queremos então passamos para o próxima "iteração"
		if((strcmp(str , "f1") != 0) && (strcmp(str , "f2") != 0)){
            printf("Client can only write f1 or f2\n");
            continue; 
        }

		if ((n = read(fd_r, &f_return, sizeof(&f_return))) <= 0){ 
			perror("read ");
			exit(-1);
		}
		printf("The value return from the server was: %d   (%d bytes)\n", f_return, n);
	}

    return 0;
}
