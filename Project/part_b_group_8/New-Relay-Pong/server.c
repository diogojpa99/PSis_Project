#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "relay-pong.h"
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include <unistd.h>


client *client_list;
time_t t_last_snd, t_curr;

ball_position_t ball;
pthread_mutex_t mx_ball;

void *thread_socket(void *arg){
    int sock_fd = *(int*) arg, remote_port, nbytes;
    struct sockaddr_in new_client_addr, client_addr;
    socklen_t new_client_addr_size = sizeof(struct sockaddr_in);
    char remote_addr[16];
    message_t in_msg, out_msg;
    client *ptr1 = NULL;
    	
    client_addr.sin_family = AF_INET;
    
    while(1) {
		nbytes = recvfrom(sock_fd, &in_msg, sizeof(in_msg), 0, (struct sockaddr *) &new_client_addr, &new_client_addr_size);
		if(nbytes == sizeof(message_t) && check_message(in_msg)== 0){
			switch(in_msg.type){
				case conn:
					// Add new client to list.
					remote_port = ntohs(new_client_addr.sin_port);
					inet_ntop(AF_INET, &new_client_addr.sin_addr, remote_addr, 16);
					pthread_mutex_lock(&mx_client_list);
					client_list = add_new_client(client_list, remote_addr, remote_port);
					pthread_mutex_unlock(&mx_client_list);
					// If it's the only client in our list, send send_ball message.
					if(client_list->next == NULL){
						pthread_mutex_lock(&mx_active_player);
						active_player = client_list;
						pthread_mutex_unlock(&mx_active_player);
						out_msg.type = snd_ball;
						pthread_mutex_lock(&mx_ball);
						place_ball_random(&ball);
						pthread_mutex_unlock(&mx_ball);
                        copy_ball(&out_msg.ball_pos, &ball);
						inet_pton(AF_INET, client_list->addr, &client_addr.sin_addr);
						client_addr.sin_port = htons(client_list->port);
						sendto(sock_fd, &out_msg, sizeof(out_msg), 0, (struct sockaddr*) &client_addr, sizeof(client_addr));
                        pthread_mutex_lock(&mx_t_last_snd);
						t_last_snd = time(NULL);
						pthread_mutex_unlock(&mx_t_last_snd);
                    }
					break;
				case snd_ball:
					break;
				case mv_ball:
					// Send the same message to all inactive players.
					ptr1 = client_list;
					pthread_mutex_lock(&mx_ball);
                    copy_ball(&ball, &in_msg.ball_pos);
					pthread_mutex_unlock(&mx_ball);
                    copy_ball(&out_msg.ball_pos, &ball);
					while(ptr1 !=NULL ){
						if(ptr1 != active_player){
							inet_pton(AF_INET, ptr1->addr, &client_addr.sin_addr);
							client_addr.sin_port = htons(ptr1->port);
							out_msg.type = mv_ball;
							sendto(sock_fd, &out_msg, sizeof(out_msg), 0, (struct sockaddr*) &client_addr, sizeof(client_addr));
						}
						ptr1 = ptr1->next;
					}
					break;
				case disconn:
                    // Remove client from list.
                    remote_port = ntohs(new_client_addr.sin_port);
					inet_ntop(AF_INET, &new_client_addr.sin_addr, remote_addr, 16);
                    // If that client is the active player
                    if( strcmp( active_player->addr, remote_addr)==0 && active_player->port == remote_port ){
                        /*
						pthread_mutex_lock(&mx_ball);
						copy_ball(&ball, &in_msg.ball_pos);
						pthread_mutex_unlock(&mx_ball);
                        */
						copy_ball(&out_msg.ball_pos, &ball);
                        pthread_mutex_lock(&mx_active_player);
						active_player = next_player(client_list);
						pthread_mutex_unlock(&mx_active_player);

                    }
					pthread_mutex_lock(&mx_client_list);
                    client_list = remove_client(client_list, remote_addr, remote_port);
					pthread_mutex_unlock(&mx_client_list);

					if(active_player != NULL){
						inet_pton(AF_INET, active_player->addr, &client_addr.sin_addr);
						client_addr.sin_port = htons(active_player->port);
						out_msg.type = snd_ball;
						sendto(sock_fd, &out_msg, sizeof(message_t), 0, (struct sockaddr*) &client_addr, sizeof(client_addr));
						pthread_mutex_lock(&mx_t_last_snd);
						t_last_snd = time(NULL);
						pthread_mutex_unlock(&mx_t_last_snd);
					}

					break;
				default:
					break;
			}
		}

	}

    return NULL;
}

/*
void *thread_time(void *arg){
	return NULL;
}
*/

int main(){
    int sock_fd;
    pthread_t _thread_socket, _thread_time;

    client_list = active_player = NULL;
	t_curr = t_last_snd = time(NULL);

    sock_fd= socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}

	struct sockaddr_in local_addr, client_addr;
	client_addr.sin_family = AF_INET;
    local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(SOCK_PORT);
	local_addr.sin_addr.s_addr = INADDR_ANY;
	message_t out_msg;
    socklen_t new_client_addr_size = sizeof(struct sockaddr_in);

	int err = bind(sock_fd, (struct sockaddr *)&local_addr, sizeof(local_addr));
	if(err == -1) {
		perror("bind");
		exit(-1);
	}

	// Initialize mutexes
	pthread_mutex_init(&mx_client_list, NULL);
	pthread_mutex_init(&mx_t_last_snd, NULL);
	pthread_mutex_init(&mx_t_curr, NULL);
	pthread_mutex_init(&mx_active_player, NULL);
	pthread_mutex_init(&mx_ball, NULL);

    // Create thread for receiving messages.
    pthread_create(&_thread_socket, NULL, thread_socket, &sock_fd);
	//pthread_create(&_thread_time, NULL, thread_time, NULL);

    while(1){
		t_curr = time(NULL);
        if( t_curr - t_last_snd >= 10 ){
			pthread_mutex_lock(&mx_t_last_snd);
			t_last_snd = time(NULL);
			pthread_mutex_unlock(&mx_t_last_snd);
			if( active_player != NULL && client_list->next != NULL ){
				// order current player to release the ball
				inet_pton(AF_INET, active_player->addr, &client_addr.sin_addr);
				client_addr.sin_port = htons(active_player->port);
				out_msg.type = rls_ball;
				copy_ball(&out_msg.ball_pos, &ball);
				sendto(sock_fd, &out_msg, sizeof(message_t), 0, (struct sockaddr*) &client_addr, sizeof(client_addr));
				// send ball to the next player.
				pthread_mutex_lock(&mx_active_player);
				active_player = next_player(client_list);
				pthread_mutex_unlock(&mx_active_player);
				if(active_player != NULL){
					inet_pton(AF_INET, active_player->addr, &client_addr.sin_addr);
					client_addr.sin_port = htons(active_player->port);
					out_msg.type = snd_ball;
					copy_ball(&out_msg.ball_pos, &ball);
					sendto(sock_fd, &out_msg, sizeof(message_t), 0, (struct sockaddr*) &client_addr, sizeof(client_addr));
				}
			}
        }
		sleep(1);
    }

}