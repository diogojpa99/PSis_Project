#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "relay-pong.h"

#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>

int main(){

    int sock_fd;

    sock_fd= socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_fd == -1){
		perror("socket: ");
		exit(-1);
	}

	struct sockaddr_in local_addr, new_client_addr, client_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(SOCK_PORT);
	local_addr.sin_addr.s_addr = INADDR_ANY;
	client_addr.sin_family = AF_INET;
    socklen_t new_client_addr_size = sizeof(struct sockaddr_in);

	int err = bind(sock_fd, (struct sockaddr *)&local_addr, sizeof(local_addr));
	if(err == -1) {
		perror("bind");
		exit(-1);
	}

    client *client_list, *ptr1, *ptr2, *active_player = NULL;
	char remote_addr[12];
	int remote_port;
	message_t in_msg, out_msg;
	int nbytes;

	while(1) {
		nbytes = recvfrom(sock_fd, &in_msg, sizeof(in_msg), 0, (struct sockaddr *) &new_client_addr, &new_client_addr_size);
		if(nbytes == sizeof(message_t) && check_message(in_msg)== 0){
			switch(in_msg.type){
				case conn:
					// Add new client to list.
					remote_port = ntohs(new_client_addr.sin_port);
					inet_ntop(AF_INET, &new_client_addr.sin_addr, remote_addr, 12);
					client_list = add_new_client(client_list, remote_addr, remote_port);
					// If its the only client in our list, send send_ball message.
					if(client_list->next == NULL){
						active_player = client_list;
						out_msg.type = snd_ball;
						place_ball_random(&out_msg.ball_pos);
						inet_pton(AF_INET, client_list->addr, &client_addr.sin_addr);
						client_addr.sin_port = htons(client_list->port);
						sendto(sock_fd, &out_msg, sizeof(out_msg), 0, (struct sockaddr*) &client_addr, sizeof(client_addr));
					}
					break;
				case rls_ball:
					// Give control of the ball to the next player.

					// If there is a player after the current one, then he is the new active player.
					// If not, we go back to the to of the list.
					if(active_player->next != NULL)
						active_player = active_player->next;
					else
						active_player = client_list;

					// Gather the new active player's address information and sent the snd_ball message.
					inet_pton(AF_INET, active_player->addr, &client_addr.sin_addr);
					client_addr.sin_port = htons(active_player->port);
					out_msg.type = snd_ball;
					copy_ball(&out_msg.ball_pos, &in_msg.ball_pos);
					sendto(sock_fd, &out_msg, sizeof(message_t), 0, (struct sockaddr*) &client_addr, sizeof(client_addr));
					break;
				case snd_ball:
					break;
				case mv_ball:
					// Send the same message to all inactive players.
					ptr1 = client_list;
					while(ptr1 !=NULL ){
						if(ptr1 != active_player){
							inet_pton(AF_INET, ptr1->addr, &client_addr.sin_addr);
							client_addr.sin_port = htons(ptr1->port);
							out_msg.type = mv_ball;
							copy_ball(&out_msg.ball_pos, &in_msg.ball_pos);
							sendto(sock_fd, &out_msg, sizeof(out_msg), 0, (struct sockaddr*) &client_addr, sizeof(client_addr));
						}
						ptr1 = ptr1->next;
					}
					break;
				case disconn:
					// Remove active player from list and pass the ball to the next one.
					ptr1 = client_list;
					while(ptr1 != active_player && ptr1 != NULL){
						ptr2=ptr1;
						ptr1=ptr1->next;
					}

					if(ptr1 != client_list)
						ptr2->next = ptr1->next;
					else
						client_list = client_list->next;

					active_player = active_player->next;
					free(ptr1);
					if(active_player == NULL)
						active_player = client_list;

					// If there is a player left to play, send snd_ball.
					if(active_player != NULL){
						inet_pton(AF_INET, active_player->addr, &client_addr.sin_addr);
						client_addr.sin_port = htons(active_player->port);
						out_msg.type = snd_ball;
						copy_ball(&out_msg.ball_pos, &in_msg.ball_pos);
						sendto(sock_fd, &out_msg, sizeof(message_t), 0, (struct sockaddr*) &client_addr, sizeof(client_addr));
					}
					break;
				default:
					break;
			}
		}

	}

}