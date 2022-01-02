#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "relay-pong.h"

#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Error: Missing recipient IP.\n");
        exit(1);
    }

    struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SOCK_PORT);
    if( inet_pton(AF_INET, argv[1], &server_addr.sin_addr) < 1){
		printf("Error: Not a valid IP address: \n");
		exit(-1);
	}

    message_t message;
    int sock_fd;
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_fd==-1){
        perror("socket: ");
        exit(-1);
    }

    // Connect to server
    message.type = conn;
    sendto(sock_fd, &message, sizeof(message_t), 0, (struct sockaddr*) &server_addr, sizeof(server_addr));

    initscr();		    	/* Start curses mode 		*/
	cbreak();				/* Line buffering disabled	*/
    keypad(stdscr, TRUE);   /* We get F1, F2 etc..		*/
	noecho();			    /* Don't echo() while we do getch */

    /* Create game window */
    WINDOW * my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(my_win, 0 , 0);	
	wrefresh(my_win);
    keypad(my_win, true);
    /* Create message window */
    message_win = newwin(5, WINDOW_SIZE+10, WINDOW_SIZE, 0);
    box(message_win, 0 , 0);	
	wrefresh(message_win);

    /*
    new_paddle(&paddle, PADLE_SIZE);
    draw_paddle(my_win, &paddle, true);

    place_ball_random(&ball);
    draw_ball(my_win, &ball, true);
    */

    typedef enum state{wait, play} state;
    int key;
    state currstate = wait, nextstate = wait;
    ball_position_t ball;
    paddle_position_t paddle;

    while (1) {
        switch(currstate){
            case wait:
                mvwprintw(message_win, 1,1,"          ");
                mvwprintw(message_win, 2,1,"                           ");
                recv(sock_fd, &message, sizeof(message_t), 0);
                if(message.type == mv_ball){
                    draw_ball(my_win, &ball, false);
                    copy_ball(&ball, &message.ball_pos);
                    draw_ball(my_win, &ball, true);
                    //nextstate = wait;
                }
                else if(message.type == snd_ball){
                    draw_ball(my_win, &ball, false);
                    copy_ball(&ball, &message.ball_pos);
                    draw_ball(my_win, &ball, true);
                    new_paddle(&paddle, PADLE_SIZE);
                    draw_paddle(my_win, &paddle, true);
                    nextstate = play;
                }
                break;
            case play:
                mvwprintw(message_win, 1,1, "PLAY STATE");
                mvwprintw(message_win, 2,1,"use arrow to control paddle");
                key = wgetch(my_win);
                if(key == 'r'){
                    // Release ball.
                    message.type = rls_ball;
                    copy_ball(&message.ball_pos, &ball);
                    sendto(sock_fd, &message, sizeof(message_t), 0, (struct sockaddr*) &server_addr, sizeof(server_addr));
                    draw_paddle(my_win, &paddle, false);
                    nextstate = wait;
                }
                else if(key == 'q'){
                    // Disconnect from server
                    message.type = disconn;
                    sendto(sock_fd, &message, sizeof(message_t), 0, (struct sockaddr*) &server_addr, sizeof(server_addr));
                    // Quit
                    exit(0);
                }
                else{
                    draw_ball(my_win, &ball, false);
                    move_ball(&ball, paddle); // adicionou-se paddle como argumento
                    draw_paddle(my_win, &paddle, false);
                    move_paddle (&paddle, key, &ball);
                    //draw_paddle(my_win, &paddle, true);
                    //draw_ball(my_win, &ball, false);
                    //moove_ball(&ball, paddle); // adicionou-se paddle como argumento
                    draw_ball(my_win, &ball, true);
                    draw_paddle(my_win, &paddle, true);
                    // Send mv_ball
                    message.type = mv_ball;
                    copy_ball(&message.ball_pos, &ball);
                    sendto(sock_fd, &message, sizeof(message_t), 0, (struct sockaddr*) &server_addr, sizeof(server_addr));
                }
                break;
        }
        wrefresh(message_win);
        wrefresh(my_win);
        currstate = nextstate;
    }
}