#include "remote-char.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include "sock_dg.h"

#include<netinet/in.h>
#include <arpa/inet.h>


#include <ncurses.h>


#define WINDOW_SIZE 15

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
    int fd;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd==-1){
        perror("socket: ");
        exit(-1);
    }

    struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SOCK_PORT);
    char linha[100];
    printf("What is the network address of the server? ");
	do{
        fgets(linha, 100, stdin);
        linha[strlen(linha)-1] = '\0';	
	    if( inet_pton(AF_INET, linha, &server_addr.sin_addr) < 1){
	    	printf("Not a valid address. Try again. ");
	    }
    }while(inet_pton(AF_INET, linha, &server_addr.sin_addr) < 1);

    char c;

    /*
    printf("Insert the character you want to control: ");
    if ( (c = fgetc(stdin)) == EOF ){
        printf("Error reading character\n");
        exit(-1);
    }
    */

    // send connection message

    //Definir a estrutura message
    message m;

    m.c = (int) c;
    m.msg_type = 3; // remote-display-connection
    int nbytes;
    do{
        nbytes = sendto(fd, &m, sizeof(message), 0, (const struct sockaddr *) &server_addr, sizeof(server_addr));
        printf("%d bytes sent.\n", nbytes);
    }while(nbytes <= 0);

	initscr();		    	
	cbreak();				
    keypad(stdscr, TRUE);   
	noecho();			    

    /* creates a window and draws a border */
    WINDOW * my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(my_win, 0 , 0);	
	wrefresh(my_win);

    int ch;
    int pos_x;
    int pos_y;
    int n_bytes;

    while (1){
        n_bytes = recv(fd, &m, sizeof(message), 0);
        if(m.msg_type == 2){
            
            /* delete old mark*/
            pos_x = m.x;
            pos_y = m.y;
            ch = m.c;
            wmove(my_win, pos_x, pos_y);
            waddch(my_win,' ');
            
            /* draw mark on new position */
            new_position(&m.x, &m.y, m.direction);	
            pos_x = m.x;
            pos_y = m.y;
            ch = m.c;
            wmove(my_win, pos_x, pos_y);
            waddch(my_win,ch| A_BOLD);
            
            wrefresh(my_win);	
        }
    }
  	endwin();			/* End curses mode		  */

	return 0;
}