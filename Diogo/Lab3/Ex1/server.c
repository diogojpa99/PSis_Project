#include <ncurses.h>
#include "remote-char.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  
#include <stdlib.h>
#define WINDOW_SIZE 15


direction_t random_direction(){
    return  random()%4;
}

typedef struct client{
    int c;
    int x;
    int y;
    struct client *next;
}client;



client *New_client(client *head, int ch, int x, int y);

client *Search_list(client *head, int ch);


void new_position(int* x, int *y, direction_t direction){
    switch (direction)
    {
    case UP:
        (*x) --;
        if(*x ==0)
            *x = 2;
        break;
    case DOWN:
        (*x) ++;
        if(*x ==WINDOW_SIZE-1)
            *x = WINDOW_SIZE-3;
        break;
    case LEFT:
        (*y) --;
        if(*y ==0)
            *y = 2;
        break;
    case RIGHT:
        (*y) ++;
        if(*y ==WINDOW_SIZE-1)
            *y = WINDOW_SIZE-3;
        break;
    default:
        break;
    }
}

int main()
{	
    // Feito por nós
    message m;
    client *listHead = NULL, *ptr =NULL;
    
	// TODO_3
    // create and open the FIFO for reading
    int fd;

	while((fd = open(FIFO, O_RDONLY)) == -1){
	  if(mkfifo(FIFO, 0666)!=0){
			printf("Problem creating the FIFO\n");
			exit(-1);
	  }else{
		  printf("FIFO created\n");
	  }
	}
	printf("FIFO just opened for reading\n");


    // ncurses initialization
	initscr();		    	
	cbreak();				
    keypad(stdscr, TRUE);   
	noecho();			    


    /* creates a window and draws a border */
    WINDOW * my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(my_win, 0 , 0);	
	wrefresh(my_win);

    /* information about the character */
    int ch;
    int pos_x;
    int pos_y;



    direction_t  direction;

    while (1)
    {
        // TODO_7
        // receive message from the clients
        read(fd, &m, sizeof(message));



        //TODO_8
        // process connection messages
        if( m.msg_type == 0){ //Connection
            ch = m.c;
            pos_x = pos_y = WINDOW_SIZE/2;
            wmove(my_win, pos_x, pos_y);
            waddch(my_win,ch| A_BOLD);

            listHead = New_client(listHead, ch, pos_x, pos_y);

        } else if ( m.msg_type == 1){ //movement
            // TODO_11
            // process the movement message

            ptr = Search_list(listHead, m.c);

            if(ptr==NULL) exit(1);


            /*deletes old place */
            wmove(my_win, ptr->x, ptr->y);
            waddch(my_win,' ');
            
            /* draw mark on new position */
            new_position(&ptr->x, &ptr->y, m.direction);
            wmove(my_win, ptr->x, ptr->y);
            waddch(my_win,ch| A_BOLD);

        }	

        wrefresh(my_win);
    }
  	endwin();			/* End curses mode		  */

	return 0;
}

client *New_client(client *head, int ch, int x, int y){

    client *new = NULL, *ptr;

    if ((new = (client*) malloc(sizeof(client))) == NULL) 
        exit(1);

    if (head == NULL){
        head = new;
    } else{
        if ( (ptr = Search_list(head, ch)) == NULL){

            new->c = ch;
            new->x = x;
            new->y = y;

            //Inserção na cabeça
            new->next= head;
            head = new;
            return new;

        } else{
            free(new);
            return ptr;
        }
    }

    return new;
}


client *Search_list(client *head, int ch){

    client *auxT = head;

    if( auxT->c == ch)
        return auxT;        
    while ( auxT->next!= NULL){
        auxT = auxT-> next;
        if( auxT->c == ch)
            return auxT;    
    }

    return NULL;
}