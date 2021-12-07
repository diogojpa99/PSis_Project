#include <ncurses.h>
#include "remote-char.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  
#include <ctype.h> 
#include <stdlib.h>
 

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

    printf("Insert the character you want to control: ");
    if ( (c = fgetc(stdin)) == EOF ){
        printf("Error reading character\n");
        exit(-1);
    }

    // TODO_6
    // send connection message

    //Definir a estrutura message
    message m;

    m.c = c;
    m.msg_type = 0; //connection

    write(fd, &m, sizeof(message));

    /*********************************/


	initscr();			/* Start curses mode 		*/
	cbreak();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();			/* Don't echo() while we do getch */

    
    int ch;
    int n = 0;
    direction_t direction;


    do
    {   

        mvprintw(1,0,"You can move the character");

    	ch = getch();		
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
        m.msg_type = 1; // Movement
        m.direction = direction;

        //TODO_10
        //send the movement message
        write(fd, &m, sizeof(message));

        /********************************/


    }while(ch != 27);
    
    
  	endwin();			/* End curses mode		  */

	return 0;
}