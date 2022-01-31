#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "relay-pong.h"

#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include <unistd.h>

pthread_mutex_t mutex_ball, mutex_paddle, mutex_my_win, mutex_message_win /* ,mutex_server_addr, mutex_state */; 
pthread_t _thread_recv, _thread_ball;

state currstate, nextstate;
ball_position_t ball;
paddle_position_t paddle;
struct sockaddr_in server_addr;


/* argument: address containing fd */
void *thread_ball(void *arg){
    int sock_fd = *(int*) arg;
    message_t message;

    while(1){
        sleep(1);

        pthread_mutex_lock(&mutex_my_win);
        draw_ball(my_win, &ball, false);
        pthread_mutex_unlock(&mutex_my_win);
        
        pthread_mutex_lock(&mutex_ball);
        move_ball(&ball, paddle);
        pthread_mutex_unlock(&mutex_ball);
        
        pthread_mutex_lock(&mutex_my_win);
        draw_ball(my_win, &ball, true);
        pthread_mutex_unlock(&mutex_my_win);
        
        // Send mv_ball.
        message.type = mv_ball;
        copy_ball(&message.ball_pos, &ball);
        sendto(sock_fd, &message, sizeof(message_t), 0, (struct sockaddr*) &server_addr, sizeof(server_addr));
    }
}

/* argument: address containing fd */
void *thread_recv(void * arg){
    int sock_fd = *(int*) arg;
    int nbytes;
    message_t message;
    while(1){
        nbytes = recv(sock_fd, &message, sizeof(message_t), 0);
        if(nbytes == sizeof(message_t) && check_message(message)==0){
            if(message.type == mv_ball){
                // Update the ball on the screen.
                pthread_mutex_lock(&mutex_my_win);
                draw_ball(my_win, &ball, false);
                pthread_mutex_unlock(&mutex_my_win);

                pthread_mutex_lock(&mutex_ball);
                copy_ball(&ball, &message.ball_pos);
                pthread_mutex_unlock(&mutex_ball);

                pthread_mutex_lock(&mutex_my_win);
                draw_ball(my_win, &ball, true);
                pthread_mutex_unlock(&mutex_my_win);
            }
            else if(message.type == snd_ball){
                // Update the ball on the screen.
                pthread_mutex_lock(&mutex_my_win);
                draw_ball(my_win, &ball, false);
                pthread_mutex_unlock(&mutex_my_win);

                pthread_mutex_lock(&mutex_ball);
                copy_ball(&ball, &message.ball_pos);
                pthread_mutex_unlock(&mutex_ball);

                pthread_mutex_lock(&mutex_my_win);
                draw_ball(my_win, &ball, true);
                pthread_mutex_unlock(&mutex_my_win);

                // Call ball movement thread
                pthread_create(&_thread_ball, NULL, thread_ball, &sock_fd);

                // Create a new paddle.
                // A new paddle is created each time the client gets the ball in order to make sure one doesn't land on top of the other.
                pthread_mutex_lock(&mutex_paddle);
                new_paddle(&paddle, PADLE_SIZE, ball);
                pthread_mutex_unlock(&mutex_paddle);

                pthread_mutex_lock(&mutex_my_win);
                draw_paddle(my_win, &paddle, true);
                pthread_mutex_unlock(&mutex_my_win);

                // Enter play state.
                pthread_mutex_lock(&mutex_message_win);
                mvwprintw(message_win, 1,1, "PLAY STATE");
                mvwprintw(message_win, 2,1,"use arrow to control paddle");
                wrefresh(message_win);
                pthread_mutex_unlock(&mutex_message_win);
                nextstate = play;
            }else if(message.type == rls_ball){
                // Delete paddle from the screen, stop ball movement and enter wait state
                pthread_mutex_lock(&mutex_my_win);
                draw_paddle(my_win, &paddle, false);
                pthread_mutex_unlock(&mutex_my_win);

                // Kill ball movement thread.
                pthread_cancel(_thread_ball);

                // Clear message_win.
                pthread_mutex_lock(&mutex_message_win);
                mvwprintw(message_win, 1,1,"          ");
                mvwprintw(message_win, 2,1,"                           ");
                wrefresh(message_win);
                pthread_mutex_unlock(&mutex_message_win);
                // Delete paddle from the screen.
                pthread_mutex_lock(&mutex_my_win);
                draw_paddle(my_win, &paddle, false);
                wrefresh(my_win);
                pthread_mutex_unlock(&mutex_my_win);

                nextstate = wait;
            }
        }
    }
}

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Error: Missing recipient IP.\n");
        exit(1);
    }

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
    my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(my_win, 0 , 0);	
	wrefresh(my_win);
    keypad(my_win, true);
    /* Create message window */
    message_win = newwin(5, WINDOW_SIZE+10, WINDOW_SIZE, 0);
    box(message_win, 0 , 0);	
	wrefresh(message_win);
    /* Initialize mutexes */
    pthread_mutex_init(&mutex_my_win, NULL);
    pthread_mutex_init(&mutex_message_win, NULL);
    pthread_mutex_init(&mutex_ball, NULL);
    /* pthread_mutex_init(&mutex_server_addr, NULL); */

    // Create communication thread
    pthread_create(&_thread_recv, NULL, thread_recv, &sock_fd);

    int key;
    time_t t_snd_ball, t;
    int nbytes;

    currstate = wait, nextstate = wait;

    while (1) {
        key = wgetch(my_win);
        if(key == 'q'){
            // Disconnect from server
            message.type = disconn;
            sendto(sock_fd, &message, sizeof(message_t), 0, (struct sockaddr*) &server_addr, sizeof(server_addr));
            // Quit
            endwin();
            close(sock_fd);
            exit(0);
        }
        switch(currstate){
            case wait:
                break;
            case play:
                // move ball and paddle and update the screen.
                pthread_mutex_lock(&mutex_my_win);
                draw_paddle(my_win, &paddle, false);
                pthread_mutex_unlock(&mutex_my_win);
                
                pthread_mutex_lock(&mutex_paddle);
                move_paddle (&paddle, key, &ball);
                pthread_mutex_unlock(&mutex_paddle);

                pthread_mutex_lock(&mutex_my_win);
                draw_paddle(my_win, &paddle, true);
                pthread_mutex_unlock(&mutex_my_win);
                break;
        }
        // wrefresh(my_win);
        currstate = nextstate;
    }
}
