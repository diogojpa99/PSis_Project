#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "super-pong.h"

#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>

#include <pthread.h>

WINDOW *message_win, *my_win;
ball_position_t ball;
paddle_position_t paddles[MAX_CLIENTS];
//pthread_mutex_t /*mx_message_win,*/ mx_my_win,/* mx_ball,*/ mx_paddles /*, mx_id*/;
int id;

void *thread_recv(void *arg){
    int sock_fd = *(int*) arg, nbytes;
    message_t message;

    while (1) {
        nbytes = recv(sock_fd, &message, sizeof(message_t), 0);
        // Check for size and validate the message.
        if(nbytes==sizeof(message_t) && check_message(message) == 0){
            // The client can only process one type of message - board_update. Therefore, we ignore any other types.
            if(message.type == board_update){
                // An id of -1 means the server is full.
                if(message.id == -1){
                    exit(0);
                }
                
                // Any other valid id becomes our own.
                // We will use it so that the server can identify our moves. 
                id = message.id;

                // Update the ball.
                draw_ball(my_win, &ball, false);
                draw_ball(my_win, &message.ball_pos, true);
                copy_ball(&ball, &message.ball_pos);

                // Update all the paddles.
                //pthread_mutex_lock(&mx_my_win);
                for(int i=0; i<MAX_CLIENTS; i++)
                    draw_paddle(my_win, paddles[i], ' ');
                for(int i=0; i<MAX_CLIENTS; i++){
                    if(i == id)
                        draw_paddle(my_win, message.paddle_pos[i], '=');
                    else
                        draw_paddle(my_win, message.paddle_pos[i], '_');
                }
                wrefresh(my_win);
                //pthread_mutex_unlock(&mx_my_win);
                //pthread_mutex_lock(&mx_paddles);
                copy_paddles(paddles, message.paddle_pos);
                //pthread_mutex_unlock(&mx_paddles);
                
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
            }
        }else if(nbytes==0){
            // server disconnected
            endwin();
            close(sock_fd);
            printf("Lost connection to server.\n");
            exit(-1);
        }
    }
}

int main(int argc, char *argv[]){

    pthread_t _thread_recv;

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
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd==-1){
        perror("socket: ");
        exit(-1);
    }
    
    // Connect to Server
    int err = connect(sock_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));
    if(err == -1){
        perror("connect");
        exit(-1);
    }

    initscr();		    	/* Start curses mode 		*/
	cbreak();				/* Line buffering disabled	*/
    keypad(stdscr, TRUE);   /* We get F1, F2 etc..		*/
	noecho();			    /* Don't echo() while we do getch */

    /* Create game window */
    my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(my_win, 0 , 0);	
	wrefresh(my_win);
    keypad(my_win, true);
    /* Create message window */
    message_win = newwin(MAX_CLIENTS+2, WINDOW_SIZE+10, WINDOW_SIZE, 0);
    box(message_win, 0 , 0);	
	wrefresh(message_win);
    wrefresh(my_win);

    // Initialize mutexes
    //pthread_mutex_init(&mx_message_win, NULL);
    //pthread_mutex_init(&mx_my_win, NULL);
    //pthread_mutex_init(&mx_ball, NULL);
    //pthread_mutex_init(&mx_paddles, NULL);
    //pthread_mutex_init(mx_id);*/

    // Create thread for receiving messages from the server
    pthread_create(&_thread_recv, NULL, thread_recv, (void*) &sock_fd);

    int key;
    while (1) {
        // Read the player's next command.
        //pthread_mutex_lock(&mx_my_win);
        key = wgetch(my_win);
        //pthread_mutex_unlock(&mx_my_win);
        message.key = key;
        message.id = id;
        // If the user wishes to quit, exit.
        if(key=='q'){
            endwin();
            close(sock_fd);
            exit(0);
        }
        else
            message.type = paddle_move;
        if( send(sock_fd, &message, sizeof(message_t), 0) == -1 ){
            // server disconnected
            endwin();
            close(sock_fd);
            printf("Lost connection to server.\n");
            exit(-1);
        }
    }

    return 0;
}