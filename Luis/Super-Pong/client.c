#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "super-pong.h"

#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>

int main(int argc, char *argv[]){
    // Check the number of command line arguments.
    if(argc != 2){
        printf("Error: Missing recipient IP.\n");
        exit(1);
    }

    // Store server information in a struct sockaddr_in
    struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SOCK_PORT);
    // If the provided address is invalid, exit.
    if( inet_pton(AF_INET, argv[1], &server_addr.sin_addr) < 1){
		printf("Error: Not a valid IP address: \n");
		exit(-1);
	}

    message_t message;
    int sock_fd, nbytes;
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_fd==-1){
        perror("socket: ");
        exit(-1);
    }

    // Connect to Server
    message.type = conn;
    nbytes = sendto(sock_fd, &message, sizeof(message_t), 0, (struct sockaddr*) &server_addr, sizeof(server_addr));

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
    WINDOW * message_win = newwin(MAX_CLIENTS+2, WINDOW_SIZE+10, WINDOW_SIZE, 0);
    box(message_win, 0 , 0);	
	wrefresh(message_win);

    int key, id;
    ball_position_t ball;
    paddle_position_t paddles[MAX_CLIENTS];

    while (1) {
        nbytes = recv(sock_fd, &message, sizeof(message_t), 0);
        // Check for size and validate the message.
        if(nbytes==sizeof(message_t) && check_message(message) == 0){
            // The client can only process one type of message - board_update. Therefore, we ignore any other types.
            if(message.type == board_update){
                // An id of -1 means the server is full.
                if(message.id == -1)
                    exit(0);
                
                // Any other valid id becomes our own.
                // We will use it so that the server can identify our moves. 
                id = message.id;

                // Update the ball.
                draw_ball(my_win, &ball, false);
                draw_ball(my_win, &message.ball_pos, true);
                copy_ball(&ball, &message.ball_pos);

                // Update all the paddles.
                for(int i=0; i<MAX_CLIENTS; i++)
                    draw_paddle(my_win, paddles[i], ' ');
                for(int i=0; i<MAX_CLIENTS; i++){
                    if(i == id)
                        draw_paddle(my_win, message.paddle_pos[i], '=');
                    else
                        draw_paddle(my_win, message.paddle_pos[i], '_');
                }
                copy_paddles(paddles, message.paddle_pos);
                
                // Update the scoreboard.
                for(int i=0; i<MAX_CLIENTS; i++){
                    if(message.score[i]>=0)
                        mvwprintw(message_win,i+1,1, "P%d - %d", i+1, message.score[i]);
                    else
                        mvwprintw(message_win,i+1,1, "          ", i+1);
                }
                // Draw an arrow in order for the user to know his player number.
                mvwprintw(message_win, id+1, 8, "<---");

                wrefresh(message_win);
                wrefresh(my_win);

                // Read the player's next command.
                key = wgetch(my_win);
                message.key = key;
                message.id = id;
                // If the user wishes to quit, disconnect from server and exit.
                if(key=='q'){
                    message.type = disconn;
                    sendto(sock_fd, &message, sizeof(message_t), 0, (struct sockaddr *) &server_addr, sizeof(server_addr));
                    exit(0);
                }
                else
                    message.type = paddle_move;
                sendto(sock_fd, &message, sizeof(message_t), 0, (struct sockaddr *) &server_addr, sizeof(server_addr));

            }
        }
    }
}