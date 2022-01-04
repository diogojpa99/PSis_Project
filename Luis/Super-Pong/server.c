#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "super-pong.h"

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

    int registered_players = 0, players_ready=0, score[MAX_CLIENTS], remote_port;
    client *client_list, *ptr1, *ptr2, *active_player = NULL;
	char remote_addr[12];
	message_t in_msg, out_msg;
    ball_position_t ball;
    paddle_position_t paddles[MAX_CLIENTS];

    for (int i=0;i<MAX_CLIENTS;i++){
        score[i]=-1;
        paddles[i].x = paddles[i].y = -1;
    }
    
    place_ball_random(&ball);

    while (1) {
        printf("Waiting for message...\n");
		recvfrom(sock_fd, &in_msg, sizeof(in_msg), 0, (struct sockaddr *) &new_client_addr, &new_client_addr_size);
		printf("\tReceived message.\n");
		switch(in_msg.type){
			case conn:
				if(registered_players <= MAX_CLIENTS){
                    // Add new client to list.
                    remote_port = ntohs(new_client_addr.sin_port);
                    inet_ntop(AF_INET, &new_client_addr.sin_addr, remote_addr, 12);
                    printf("\t%s %d\n", remote_addr, remote_port);
                    client_list = add_new_client(client_list, remote_addr, remote_port, score, paddles);
                    registered_players ++;
                }
                // Send board update.
                /* TO DO */
                break;
            case disconn:
                // Remove client from list.
                ptr1 = client_list;
                ptr2 = NULL;
                while(ptr1!=NULL && ptr1->id != in_msg.id){
                    ptr2 = ptr1;
                    ptr1 = ptr1->next;
                }
                if(ptr2 == NULL){
                    client_list = ptr1->next;
                    free(ptr1);
                }else{
                    ptr2->next = ptr1->next;
                    free(ptr1);
                }
                score[in_msg.id]= -1;
                paddles[in_msg.id].x = paddles[in_msg.id].y = -1;
                registered_players--;
                break;
            case paddle_move:
                move_paddle(paddles, in_msg.key, &ball, in_msg.id);
                players_ready++;
                if(players_ready == registered_players){
                    // Move the ball and update boards.
                    move_ball(&ball, paddles, score);
                    players_ready = 0;
                    /*TO DO*/
                }
                break;
        }
    }
    exit(0);
}