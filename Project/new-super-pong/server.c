#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "super-pong.h"

#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include <unistd.h>

#include <pthread.h>

int score[10], registered_players;
paddle_position_t paddles[10];
ball_position_t ball;
client *client_list = NULL;
pthread_mutex_t mx_client_list, mx_score;

void *thread_client(void* arg){
    int sock_fd = *(int*) arg;
    int id = -1;
    int nbytes;
    client *ptr1;
    message_t in_msg, out_msg;

    while (1) {
        nbytes = recv(sock_fd, &in_msg, sizeof(in_msg), 0);
        if(nbytes==sizeof(message_t) && check_message(in_msg) == 0){
            if(in_msg.id >= 0 && in_msg.id < MAX_CLIENTS){
                id = in_msg.id;
            }
            switch(in_msg.type){
                case paddle_move:
                    move_paddle(paddles, in_msg.key, &ball, in_msg.id);
                    // Send board update.
                    out_msg.type = board_update;
                    copy_ball(&out_msg.ball_pos, &ball);
                    for(int i=0; i<MAX_CLIENTS; i++){                    
                        out_msg.score[i] = score[i];
                    }
                    copy_paddles(out_msg.paddle_pos, paddles);
                    
                    pthread_mutex_lock(&mx_client_list);
                    ptr1 = client_list;
                    while(ptr1 != NULL){
                        out_msg.id = ptr1->id;
                        send(ptr1->fd, &out_msg, sizeof(message_t), 0);
                        printf("\t\tsent to fd = %d\n", ptr1->fd);
                        ptr1 = ptr1->next;
                    }
                    pthread_mutex_unlock(&mx_client_list);
                    break;
            }
        }else if(nbytes == 0){
            printf("Player %d quit. (fd = %d)\n", id+1, sock_fd);
            // Client disconnected.
            registered_players--;

            // Remove client from list.
            pthread_mutex_lock(&mx_client_list);
            client_list = remove_client(client_list, sock_fd, &id);
            pthread_mutex_unlock(&mx_client_list);
            printf("\tClient removed from list.\n");

            // Clear client score and paddle.
            if(id != -1){
                pthread_mutex_lock(&mx_score);
                score[id] = -1;
                pthread_mutex_unlock(&mx_score);
                paddles[id].x = paddles[id].y = -1;
            }

            // Close file descriptor
            close(sock_fd);

            // Kill this thread
            pthread_exit(NULL);
        }
    }
}

void *thread_accept(void *arg){
    int sock_fd = *(int*) arg;
    int newfd, remote_port, err, id;
    char remote_addr[16];
    struct sockaddr_in new_client_addr;
    socklen_t new_client_addrlen;
    message_t out_msg, in_msg;
    pthread_t _thread_client;

    new_client_addr.sin_family = AF_INET;

    while (1){
        newfd = accept(sock_fd, (struct sockaddr*) &new_client_addr, &new_client_addrlen);

        if(newfd != -1){
            if(registered_players <= MAX_CLIENTS){
                // Add new client to list.
                remote_port = ntohs(new_client_addr.sin_port);
                inet_ntop(AF_INET, &new_client_addr.sin_addr, remote_addr, 16);
                pthread_mutex_lock(&mx_client_list);
                pthread_mutex_lock(&mx_score);
                client_list = add_new_client(client_list, remote_addr, remote_port, score, paddles, &id, ball, newfd);
                pthread_mutex_unlock(&mx_client_list);
                pthread_mutex_unlock(&mx_score);
                registered_players ++;
                out_msg.id = id;
                // Call client thread
                pthread_create(&_thread_client, NULL, thread_client, &newfd);
            }
            // Send board update.
            out_msg.type = board_update;
            copy_ball(&out_msg.ball_pos, &ball);
            for(int i=0; i<MAX_CLIENTS; i++){                    
                out_msg.score[i] = score[i];
            }
            copy_paddles(out_msg.paddle_pos, paddles);
            if(registered_players > MAX_CLIENTS){
                out_msg.id=-1;
                registered_players--;
            }
            else
                out_msg.id = id;

            //send(newfd, &out_msg, sizeof(message_t), 0);
            send(client_list->fd, &out_msg, sizeof(message_t), 0);
            printf("Accept: newfd = %d\n", newfd);
        }
    }

}

void *thread_ball(void *arg){
    message_t message;
    client *ptr1;

    printf("Ball movement thread\n");

    while(1){
        sleep(1);
        printf("\tMove\n");
        pthread_mutex_lock(&mx_score);
        move_ball(&ball, paddles, score);
        pthread_mutex_unlock(&mx_score);

        // Send board update.
        message.type = board_update;
        copy_ball(&message.ball_pos, &ball);
        for(int i=0; i<MAX_CLIENTS; i++){                    
            message.score[i] = score[i];
        }
        copy_paddles(message.paddle_pos, paddles);
        
        pthread_mutex_lock(&mx_client_list);
        ptr1 = client_list;
        while(ptr1 != NULL){
            message.id = ptr1->id;
            send(ptr1->fd, &message, sizeof(message_t), 0);
            printf("\t\tsent to fd = %d\n", ptr1->fd);
            ptr1 = ptr1->next;
        }
        pthread_mutex_unlock(&mx_client_list);
    }
}

int main(){
    int sock_fd;
    pthread_t _thread_accept, _thread_ball;
    sock_fd= socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}

	struct sockaddr_in local_addr, new_client_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(SOCK_PORT);
	local_addr.sin_addr.s_addr = INADDR_ANY;
    socklen_t new_client_addr_size = sizeof(struct sockaddr_in);

	int err = bind(sock_fd, (struct sockaddr *)&local_addr, sizeof(local_addr));
	if(err == -1) {
		perror("bind");
		exit(-1);
	}

    err = listen(sock_fd, 10);
    if(err == -1) {
        perror("listen");
        exit(-1);
    }
    
    registered_players = 0;
    for(int i=0; i<MAX_CLIENTS; i++){
        score[i] = -1;
        paddles[i].x = paddles[i].y = -1;
        paddles[i].length = PADLE_SIZE;
    }

    place_ball_random(&ball);

    // Initialize mutexes
    pthread_mutex_init(&mx_client_list, NULL);
    pthread_mutex_init(&mx_score, NULL);

    // Create thread for accepting new connections.
    pthread_create(&_thread_accept, NULL, thread_accept, &sock_fd);
    pthread_create(&_thread_ball, NULL, thread_ball, NULL);

    pthread_exit(NULL);
}