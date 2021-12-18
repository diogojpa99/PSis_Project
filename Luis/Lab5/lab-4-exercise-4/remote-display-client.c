#include "remote-char.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#include<netinet/in.h>
#include <arpa/inet.h>


#include <ncurses.h>


#define WINDOW_SIZE 15
#define SOCK_PORT 5000


int main(){

    char linha[100];
   	printf("What is the network address of the recipient? ");
	fgets(linha, 100, stdin);
	linha[strlen(linha)-1] = '\0';
	
    struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SOCK_PORT);
	if( inet_pton(AF_INET, linha, &server_addr.sin_addr) < 1){
		printf("no valid address: \n");
		exit(-1);
	}   
    int sock_fd;
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_fd==-1){
        perror("socket: ");
        exit(-1);
    }

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


    remote_char_t m;
    
    char key;
    int n=0;

    while (1){
        /*
        if(m.msg_type == 6){
                pos_x = m.x;
                pos_y = m.y;
                ch = m.ch;
                wmove(my_win, pos_x, pos_y);
                waddch(my_win,ch| A_BOLD);
                wrefresh(my_win);			
        }
        */

        
        key = getch();		
        n++;
        remote_char_t m;

        switch (key)
        {
            case KEY_LEFT:
                mvprintw(0,0,"%d Left arrow is pressed", n);
                //TODO_9
                // prepare the movement message
                m.direction = LEFT;
                break;
            case KEY_RIGHT:
                mvprintw(0,0,"%d Right arrow is pressed", n);
                //TODO_9
                // prepare the movement message
                m.direction = RIGHT;
                break;
            case KEY_DOWN:
                mvprintw(0,0,"%d Down arrow is pressed", n);
                //TODO_9
                // prepare the movement message
                m.direction = DOWN;
                break;
            case KEY_UP:
                mvprintw(0,0,"%d :Up arrow is pressed", n);
                //TODO_9
                // prepare the movement message
                m.direction = UP;
                break;

            default:
                key = 'x';
                break;
        }

        //TODO_10
        //send the movement message
        if (key != 'x'){
            sendto(sock_fd, &m, sizeof(remote_char_t), 0, 
                (const struct sockaddr *)&server_addr, sizeof(server_addr));
            recv(sock_fd, &m, sizeof(remote_char_t), 0);
            if(m.msg_type==3){
                flash();
           }
        }
        refresh();
    }
  	endwin();			/* End curses mode		  */

	return 0;
}