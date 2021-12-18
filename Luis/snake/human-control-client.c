#include <ncurses.h>
#include "remote-char.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <fcntl.h>  
#include <ctype.h> 
#include <stdlib.h>
#include <time.h>

void delay(int milliseconds)
{
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
        now = clock();
}

int main()
{
   
    //TODO_4
    // create and open the FIFO for writing

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
    // read the character from the user

    char c;

    initscr();			/* Start curses mode 		*/
	cbreak();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();			/* Don't echo() while we do getch */

    printf("Press ENTER to start");
    if ( (c = fgetc(stdin)) == EOF ){
        printf("Error reading character\n");
        exit(-1);
    }

    c = 'O';

    // TODO_6
    // send connection message

    //Definir a estrutura message
    message m;

    m.c = (int) c;
    m.msg_type = 0; //connection

    write(fd, &m, sizeof(message));

    // send the default first command (DOWN)
    m.c = c;
    m.msg_type = 1;
    m.direction = DOWN;
    write(fd, &m, sizeof(message));

    /*********************************/
    
    int ch;
    int n = 0;
    int kbfd = 0;
    int sret;

    fd_set readfds;
    struct timeval timeout;

    timeout.tv_sec=0;
    timeout.tv_usec=500000;

    direction_t direction;

    do
    {   

        mvprintw(1,0,"You can move the character");

        FD_ZERO(&readfds);
        FD_SET(kbfd, &readfds);
        sret = select(2, &readfds, NULL, NULL, &timeout);
        if(sret!=0){
    	    ch = getch();
        }		
        n++;
        switch (ch)
        {
            case KEY_LEFT:
                mvprintw(0,0,"%d Left arrow is pressed", n);
                direction = LEFT;
                break;
            case KEY_RIGHT:
                mvprintw(0,0,"%d Right arrow is pressed", n);
                direction = RIGHT;
                break;
            case KEY_DOWN:
                mvprintw(0,0,"%d Down arrow is pressed", n);
                direction = DOWN;
                break;
            case KEY_UP:
                mvprintw(0,0,"%d :Up arrow is pressed", n);
                direction = UP;
                break;
            default:
                ch = 'x';
                    break;
        }
        refresh();			/* Print it on to the real screen */
        
        
        //TODO_9
        // prepare the movement message
        m.c = c;
        m.msg_type = 1; // Movement
        m.direction = direction;

        //TODO_10
        //send the movement message
        write(fd, &m, sizeof(message));

        /********************************/

        delay(500);

    }while(ch != 27);
    
    
  	endwin();			/* End curses mode		  */

	return 0;
}