#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

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

    typedef enum state{wait, play} state;
    int key;
    ball_position_t ball;
    paddle_position_t paddle;
    time_t t_snd_ball, t;
    int nbytes;

    /*
        The client is a state machine with two states:
            wait:
                the client receives mv_ball messages from the server and updates its screen.
            play:
                the client reads characters from the keyboard and reacts accordingly.
                'r' releases the ball (only after playing for 10 s);
                'q' disconnects from the server and exits;
                any other key moves the paddle and the ball. (Only arrow keys can actually move the paddle)
    */
    state currstate = wait, nextstate = wait; // Start in wait state.
    while (1) {
        switch(currstate){
            case wait:
                // Clear message_win.
                mvwprintw(message_win, 1,1,"          ");
                mvwprintw(message_win, 2,1,"                           ");
                wrefresh(message_win);
                nbytes = recv(sock_fd, &message, sizeof(message_t), 0);
                if(nbytes == sizeof(message_t) && check_message(message)==0){
                    if(message.type == mv_ball){
                        // Update the ball on the screen.
                        draw_ball(my_win, &ball, false);
                        copy_ball(&ball, &message.ball_pos);
                        draw_ball(my_win, &ball, true);
                    }
                    else if(message.type == snd_ball){
                        // Set the time at which we received the snd_ball message (t_snd_ball).
                        t_snd_ball = time(NULL);

                        // Update the ball on the screen.
                        draw_ball(my_win, &ball, false);
                        copy_ball(&ball, &message.ball_pos);
                        draw_ball(my_win, &ball, true);

                        // Create a new paddle.
                        // A new paddle is created each time the client gets the ball in order to make sure one doesn't land on top of the other.
                        new_paddle(&paddle, PADLE_SIZE, ball);
                        draw_paddle(my_win, &paddle, true);

                        // Enter play state.
                        nextstate = play;
                    }
                }
                break;
            case play:
                mvwprintw(message_win, 1,1, "PLAY STATE");
                mvwprintw(message_win, 2,1,"use arrow to control paddle");
                wrefresh(message_win);
                key = wgetch(my_win);
                t = time(NULL); // Record instant when the key is pressed.
                if(key == 'r'){
                    // If 10 seconds have passed since receiving the snd_ball message, the ball is released.
                    if(t - t_snd_ball >= 10){
                        // Send rls_ball message.
                        message.type = rls_ball;
                        copy_ball(&message.ball_pos, &ball);
                        sendto(sock_fd, &message, sizeof(message_t), 0, (struct sockaddr*) &server_addr, sizeof(server_addr));
                        draw_paddle(my_win, &paddle, false);

                        // Enter wait state.
                        nextstate = wait;
                    }
                }
                else if(key == 'q'){
                    // Disconnect from server
                    message.type = disconn;
                    sendto(sock_fd, &message, sizeof(message_t), 0, (struct sockaddr*) &server_addr, sizeof(server_addr));
                    // Quit
                    endwin();
                    exit(0);
                }
                else{
                    // move ball and paddle and update the screen.
                    draw_ball(my_win, &ball, false);
                    move_ball(&ball, paddle);
                    draw_paddle(my_win, &paddle, false);
                    move_paddle (&paddle, key, &ball);
                    draw_ball(my_win, &ball, true);
                    draw_paddle(my_win, &paddle, true);

                    // Send mv_ball.
                    message.type = mv_ball;
                    copy_ball(&message.ball_pos, &ball);
                    sendto(sock_fd, &message, sizeof(message_t), 0, (struct sockaddr*) &server_addr, sizeof(server_addr));
                }
                break;
        }
        wrefresh(my_win);
        currstate = nextstate;
    }
}