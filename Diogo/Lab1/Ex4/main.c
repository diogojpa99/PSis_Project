#include "lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* include the correct .h file */
#include <dlfcn.h>

int main(){
	int a;
	char line[100];
	char library_name[100];

	void *handle;
	char *error;


	printf("What version of the functions you whant to use?\n");
	printf("\t1 - Normal    (lib1)\n");
	printf("\t2 - Optimized (lib2)\n");
	fgets(line, 100, stdin);
	sscanf(line,"%d", &a);
	if (a == 1){
		strcpy(library_name, "./lib1.so");
		printf("running the normal versions from %s \n", library_name);
	}else{
		if(a== 2){
			strcpy(library_name, "./lib2.so");
			printf("running the better versions %s \n", library_name);
		}else{
			printf("Not running anything\n");
			exit(-1);
		}
	}

	/* load library from name library_name */
	handle = dlopen (library_name, RTLD_LAZY);

	if (!handle){
      fprintf(stderr, "%s\n", dlerror());
      exit(1);
   	}

	/* declare pointers to functions */
	void (*ptr1)() , (*ptr2)();

	/*load func_1 from loaded library */
	ptr1 = dlsym(handle, "func_1");

	if ((error = dlerror()) != NULL) {
      fprintf(stderr, "%s\n", error);
      exit(1);
   	}

	/*load func_2 from loaded library */
	ptr2 = dlsym(handle, "func_2");

	if ((error = dlerror()) != NULL) {
      fprintf(stderr, "%s\n", error);
      exit(1);
   	}

	/* call func_1 from whichever library was loaded */
	(*ptr1)();

	/* call func_2 from whichever library was loaded */
	(*ptr2)();

	dlclose(handle);

	exit(0);
}
