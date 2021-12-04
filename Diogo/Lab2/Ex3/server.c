#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>

#define BUFFER_SIZE 100


int main(){

	int n , fd, (*f)(), f_return;
	char str[BUFFER_SIZE], lib_name[BUFFER_SIZE];
    void *lib_handle;
    char *error;
   

    strcpy(lib_name, "./funcs.so");

    //Opening or creating a FIFO/PIPE
	while((fd = open("/tmp/fifo_teste", O_RDONLY)) == -1){
	  if(mkfifo("/tmp/fifo_teste", 0666)!=0){
			printf("Problem creating the fifo\n");
			exit(-1);
	  }else{
		  printf("Fifo created\n");
	  }
	}
	printf("Fifo just opened\n");


	while(1){

        //Reading from the opened PIPE
        //n é o nº de Bites lidos. Se n = -1 significa que o cliente saiu 
		if ((n = read(fd, str, BUFFER_SIZE)) <= 0){ 
			perror("read ");
			exit(-1);
		}
		printf("String read from the FIFO: %s   (%d bytes)\n", str, n);

        if((strcmp(str , "f1") != 0) && (strcmp(str , "f2") != 0)){
            printf("You can only write f1 or f2\n");
            continue; //Se o utilizador não escrever o que queremos então passamos para o próxima "iteração"
        }
        /* load library from name library_name */
        if (!(lib_handle = dlopen (lib_name, RTLD_LAZY))){
            fprintf(stderr, "%s\n", dlerror());
            exit(1);
        }

        /*load the selected function from loaded library */
        f = dlsym(lib_handle, str);
        
        if ((error = dlerror()) != NULL) {
            fprintf(stderr, "%s\n", error);
            exit(1);
        }

        /* call f from whichever library was loaded */
        f_return = (*f)();
        printf("Value return by the selected fucntion was: %d\n", f_return);

        dlclose(lib_handle);
	}
    
    return 0;
}