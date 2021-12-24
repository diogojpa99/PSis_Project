#include <stdlib.h>
#include <stdio.h>

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
    socklen_t new_client_addr_size = sizeof(struct sockaddr_in);

	int err = bind(sock_fd, (struct sockaddr *)&local_addr, sizeof(local_addr));
	if(err == -1) {
		perror("bind");
		exit(-1);
	}

    client *client_list, ptr1;
	char remote_addr[12];
	int remote_port;
	message_t message;

	while(1) {
		recvfrom(sock_fd, &message, sizeof(message), 0, (struct sockaddr *) &new_client_addr, &new_client_addr_size);
		
		switch(message.type){
			case conn:
				// Add new client to list.
				remote_port = ntohs(new_client_addr.sin_port);
				inet_ntop(AF_INET, &client_addr.sin_addr, remote_addr, 12);
				if( add_new_client(client_list, remote_addr, remote_port) == -1){
					// send shutdown??
				}
				// If only client in list, send send_ball message.
				if(client_list->next == NULL){
					message.type = snd_ball;
					place_ball_random(&message.ball_pos);
					inet_pton(AF_INET, client_list->addr, &client_addr.sin_addr);
					client_addr.sin_port = htons(client_list->port);
					sendto(sock_fd, &message, sizeof(message), 0, (struct sockaddr*) &client_addr, sizeof(client_addr));
				}
		}

	}

}