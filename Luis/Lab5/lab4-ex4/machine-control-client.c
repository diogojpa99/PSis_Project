#include "remote-char.h"
#include "sock_dg.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

int main()
{	
    //TODO_4
    int fd;

    while((fd = open(FIFO, O_WRONLY))== -1){
	  if(mkfifo(FIFO, 0666)!=0){
			printf("Problem creating the FIFO\n");
			exit(-1);
	  }else{
		  printf("FIFO created\n");
	  }
	}
	printf("FIFO just opened for writing\n");

    //TODO_5

    //int c = 'M';

    char c;
        
    printf("Insert the character you want to control: ");
    if ( (c = fgetc(stdin)) == EOF ){
        printf("Error reading character\n");
        exit(-1);
    }


    // TODO_6

        message m;

    m.c = (int) c;
    m.msg_type = 0; //connection

    write(fd, &m, sizeof(message));

    /*****************/

    int sleep_delay;
    direction_t direction;
    int n = 0;
    while (1)
    {
        sleep_delay = random()%700000;
        usleep(sleep_delay);
        direction = random()%4;
        n++;
        switch (direction)
        {
        case LEFT:
           printf("%d Going Left   ", n);
            break;
        case RIGHT:
            printf("%d Going Right   ", n);
           break;
        case DOWN:
            printf("%d Going Down   ", n);
            break;
        case UP:
            printf("%d Going Up    ", n);
            break;
        }
        //TODO_9
        m.msg_type = 1; // Movement
        m.direction = direction;


        //TODO_10
        write(fd, &m, sizeof(message));

    }

 
	return 0;
}