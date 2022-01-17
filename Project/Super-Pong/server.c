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

    /*
        score is an array containing the current scoreboard.
            score[id] is the current score of player id+1 (e.g. score[0] corresponds to player 1).
            when a certain id is free (no player is using it), score[id] is set to -1, in order to identify free ids for new players.
        
        paddles is an array containing the positions of all the paddles.
            paddles[id] refers to player id+1.
            when a certain id is free (no player is using it), paddles[id].x and paddles[id].y are set to -1, so that they don't appear on the screen.
        
        client_list is the list of active clients, containing their IP address, port and id.
    */
    int registered_players = 0, players_ready=0, score[MAX_CLIENTS], remote_port, id;
    client *client_list = NULL, *ptr1, *ptr2;
	char remote_addr[12];
	message_t in_msg, out_msg;
    ball_position_t ball;
    paddle_position_t paddles[MAX_CLIENTS];

    int nbytes;

    for (int i=0;i<MAX_CLIENTS;i++){
        score[i]=-1;
        paddles[i].x = paddles[i].y = -1;
        paddles[i].length = PADLE_SIZE;
    }
    
    place_ball_random(&ball);

    while (1) {
        nbytes = recvfrom(sock_fd, &in_msg, sizeof(in_msg), 0, (struct sockaddr *) &new_client_addr, &new_client_addr_size);
        if(nbytes==sizeof(message_t) && check_message(in_msg) == 0){
            switch(in_msg.type){
                case conn:
                    if(registered_players <= MAX_CLIENTS){
                        // Add new client to list.
                        remote_port = ntohs(new_client_addr.sin_port);
                        inet_ntop(AF_INET, &new_client_addr.sin_addr, remote_addr, 12);
                        client_list = add_new_client(client_list, remote_addr, remote_port, score, paddles, &id, ball);
                        registered_players ++;
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
                    sendto(sock_fd, &out_msg, sizeof(message_t), 0, (struct sockaddr *) &new_client_addr, sizeof(new_client_addr));
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
                    // Send board update.
                    ptr1 = client_list;
                    while(ptr1->id != in_msg.id){
                        ptr1 = ptr1->next;
                    }
                    if(ptr1 != NULL){
                        out_msg.type = board_update;
                        copy_ball(&out_msg.ball_pos, &ball);
                        for(int i=0; i<MAX_CLIENTS; i++){    
                            out_msg.score[i] = score[i];
                        }
                        copy_paddles(out_msg.paddle_pos, paddles);
                        out_msg.id = ptr1->id;
                        inet_pton(AF_INET, ptr1->addr, &client_addr.sin_addr);
                        client_addr.sin_port = htons(ptr1->port);
                        sendto(sock_fd, &out_msg, sizeof(message_t), 0, (struct sockaddr *) &client_addr, sizeof(client_addr));
                    }
                    players_ready++;
                    if(players_ready == registered_players){
                        // Move the ball.
                        move_ball(&ball, paddles, score);
                        players_ready = 0;
                    }
                    break;
            }
        }
    }
    exit(0);
}
