#include <ncurses.h>
#include "remote-char.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  
#include <stdlib.h>
#include <time.h>
#define WINDOW_SIZE 15
#define SNAKE_SIZE 10


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

client *remove_point(client *listhead, int x, int y);

client *Search_list(client *head, int ch);

void newPoint(int *point);

void increase_length(client snake[]);

void new_position(int* x, int *y, direction_t direction){
    switch (direction)
    {
    case UP:
        (*x) --;
        if(*x ==0)
            *x = WINDOW_SIZE-2;
        break;
    case DOWN:
        (*x) ++;
        if(*x ==WINDOW_SIZE-1)
            *x = 1;
        break;
    case LEFT:
        (*y) --;
        if(*y ==0)
            *y = WINDOW_SIZE-2;
        break;
    case RIGHT:
        (*y) ++;
        if(*y ==WINDOW_SIZE-1)
            *y = 1;
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
    int point[2];
    clock_t now, then;
    client snake[SNAKE_SIZE];
    int snake_length=3;
    int n=0;

    direction_t  direction;

    //now = then = clock();

    while (1)
    {
        //now = clock();

        if(n==10){
            n=0;
            newPoint(point);
            for(int i=0; i<snake_length; i++){
                if(snake[i].x==point[0] && snake[i].y==point[1]){
                    n=5;
                }
            }
            if(n==0){
                wmove(my_win, point[0], point[1]);
                waddch(my_win,'.'| A_BOLD);
                listHead = New_client(listHead, '.', point[0], point[1]);
            }
        }

        // TODO_7
        // receive message from the clients
        read(fd, &m, sizeof(message));
        n++;



        //TODO_8
        // process connection messages
        if( m.msg_type == 0){ //Connection
            ch = 'O';
            //pos_x = pos_y = WINDOW_SIZE/2;

            listHead = New_client(listHead, ch, pos_x, pos_y);

            snake[0].c = 'O';
            for(int i=0; i<SNAKE_SIZE; i++){
                if(i!=0)
                    snake[i].c = 'o';
                snake[i].x=SNAKE_SIZE-i;
                snake[i].y=2;
                if(i<snake_length){
                    wmove(my_win, snake[i].x, snake[i].y);
                    waddch(my_win,snake[i].c| A_BOLD);
                }
            }
        } else if ( m.msg_type == 1){ //movement
            // TODO_11
            // process the movement message

            ptr = Search_list(listHead, m.c);

            if(ptr!=NULL){
                /*
                wmove(my_win, ptr->x, ptr->y);
                waddch(my_win,' ');
                // draw mark on new position
                new_position(&ptr->x, &ptr->y, m.direction);
                wmove(my_win, ptr->x, ptr->y);
                waddch(my_win,ptr->c| A_BOLD);
                */
                wmove(my_win, snake[snake_length-1].x, snake[snake_length-1].y);
                waddch(my_win,' ');
                for(int i=SNAKE_SIZE-1;i>0;i--){
                    snake[i].x=snake[i-1].x;
                    snake[i].y=snake[i-1].y;
                    /*
                    wmove(my_win, snake[i].x, snake[i].y);
                    waddch(my_win,snake[i].c| A_BOLD);
                    */
                }
                new_position(&snake[0].x, &snake[0].y, m.direction);
                ptr = listHead;
                while(ptr!=NULL){
                    if(ptr->x==snake[0].x && ptr->y==snake[0].y){
                        if(ptr->c=='.')
                            if(snake_length<SNAKE_SIZE){
                                snake_length++;
                                listHead = remove_point(listHead, snake[0].x, snake[0].y);
                            }
                    }
                    ptr=ptr->next;
                }
                for(int i=0;i<snake_length;i++){
                    wmove(my_win, snake[i].x, snake[i].y);
                    waddch(my_win,snake[i].c| A_BOLD);
                }
            }
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

    new->c = ch;
    new->x = x;
    new->y = y;
    
    if (head == NULL){
        return head = new;
    } else{
            //Inserção na cabeça
            new->next= head;
            head = new;
            return new;   
    }

    return new;
}

client *remove_point(client *listhead, int x, int y){
    client *ptr, *ptr2;
    if(listhead==NULL)
        return listhead;

    ptr = listhead->next;
    ptr2 = listhead;
    if(listhead->x==x && listhead->y==y && listhead->c=='.'){
        ptr = listhead->next;
        free(listhead);
        return ptr;
    }

    while(ptr!=NULL){
        if(ptr->x==x && ptr->y==y && ptr->c=='.'){
            ptr2->next = ptr->next;
            free(ptr);
            return(ptr);
        }
        ptr2 = ptr;
        ptr = ptr->next;
    }

    return listhead;
}


client *Search_list(client *head, int ch){

    client *auxT = head;
      
    while ( auxT != NULL){
        if( auxT->c == ch)
            return auxT;   
        auxT = auxT-> next; 
    }

    return NULL;
}

void newPoint(int *point){
    point[0] = random() % (WINDOW_SIZE-2) + 1;
    point[1] = random() % (WINDOW_SIZE-2) + 1;
}

void increase_length(client snake[]){
    //if(snake[SNAKE_SIZE-1].c=='o')
      //  return;
    for(int i=0; i++; i<SNAKE_SIZE){
        if(snake[i].c!='o'){
            snake[i].c='o';
            break;
        }
    }
}