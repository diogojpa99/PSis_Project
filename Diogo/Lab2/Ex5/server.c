#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>

#include "data_struct.h"

#define BUFFER_SIZE 100


int main(){

	int n , fd_r, fd_w,  (*f)(), f_return;
	char str[BUFFER_SIZE], lib_name[BUFFER_SIZE];
    void *lib_handle;
    char *error;
    message_type message;

    strcpy(lib_name, "./funcs-ex5.so");

    //Opening or creating a FIFO/PIPE
	while((fd_r = open("/tmp/fifo_teste", O_RDONLY)) == -1){
	  if(mkfifo("/tmp/fifo_teste", 0666)!=0){
			printf("Problem creating the fifo\n");
			exit(-1);
	  }else{
		  printf("Fifo created\n");
	  }
	}
	printf("Fifo_read just opened for reading\n");

    // fd_write -- /tmp/fifo_return (write-only)
    while((fd_w = open("/tmp/fifo_return", O_WRONLY)) ==-1){
        if(mkfifo("/tmp/fifo_return", 0666)!=0){
            printf("FIFO write file not created\n");
            exit(-1);
        }
    }
    printf("FIFO write file opened successfully\n");


	while(1){

        //Reading from the opened PIPE
        //n é o nº de Bites lidos. Se n = -1 significa que o cliente saiu 
		if ((n = read(fd_r, &message, sizeof(message_type))) <= 0){ 
			perror("read ");
			exit(-1);
		}
		printf("String read from the FIFO_read: %s   (%d bytes)\n", message.f_name, n);

		//Se o utilizador não escrever o que queremos então passamos para o próxima "iteração"
		if((strcmp( message.f_name , "f1") != 0) && (strcmp( message.f_name , "f2") != 0) && (strcmp( message.f_name, "f3") != 0)){
			printf("Client can only write f1 or f2 or f3\n");
			continue; 
		}

        /* load library from name library_name */
        if (!(lib_handle = dlopen (lib_name, RTLD_LAZY))){
            fprintf(stderr, "%s\n", dlerror());
            exit(1);
        }

        /*load the selected function from loaded library */
        f = dlsym(lib_handle, message.f_name);
        
        if ((error = dlerror()) != NULL) {
            fprintf(stderr, "%s\n", error);
            exit(1);
        }

        /* call f from whichever library was loaded */
        if ( strcmp (message.f_name, "f3") ==0 ){
            f_return = (*f)(message.arg);
        } else{
            f_return = (*f)();
        }

        write (fd_w, &f_return, sizeof(&f_return));

        dlclose(lib_handle);
	}
    
    return 0;
}