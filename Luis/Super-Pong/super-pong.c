#include "super-pong.h"
#include <string.h>

/*
    The server holds a list containing information about the clients
    This function is used when a new client tries to connect to the server.
    It is also where clients get their id.
*/
client *add_new_client(client *list, char *addr, int port, int * score, paddle_position_t * paddles, int *id, ball_position_t ball){
    client *new, *p=list;
    int i = 0;

    new = (client*) malloc(sizeof(client));
    if( new == NULL ){
        return list; // There was an error. The list stays the same.
    }

    // Store the new client's data.
    while(i<MAX_CLIENTS && score[i]!=-1){
        i++;
    }
    strcpy(new->addr, addr);
    new->port = port;
    new->id = *id = i;
    new->next = list;   // Client is inserted at the head of the list.
    new_paddle(paddles, PADLE_SIZE, i, ball);
    score[i] = 0;

    return new;
}

/*
    Each time a message is received, it must be validated.
    This function validates messages acording to their type.
*/
int check_message(message_t msg){
    int r = 0;

    switch(msg.type){
        case conn:
            // No other field needs checking.
            break;
        case paddle_move:
            // Check id
            if(msg.id < 0 || msg.id >= MAX_CLIENTS)
                r = -1;
            // Any key can be pressed, but only arrow keys move the paddle.
            break;
        case board_update:
            // Check paddles and score
            for(int i=0; i<MAX_CLIENTS; i++){
                if(msg.paddle_pos[i].x < -1 || msg.paddle_pos[i].x > WINDOW_SIZE-2 || msg.paddle_pos[i].x == 0)
                    r = -1;
                if(msg.paddle_pos[i].y < -1 || msg.paddle_pos[i].y > WINDOW_SIZE-2 || msg.paddle_pos[i].y == 0)
                    r = -1;
                if(msg.paddle_pos[i].length != PADLE_SIZE)
                    r = -1;
                if(msg.score[i] < -1)
                    r = -1;
            }
            // Check id
            // In this case we allow the id to be equal to -1, because it tells the client to exit.
            if(msg.id < -1 || msg.id >= MAX_CLIENTS)
                r = -1;
            break;
        case disconn:
            // Check id
            if(msg.id < 0 || msg.id >= MAX_CLIENTS)
                r = -1;
            break;
        default:
            // The default case is equivalent to msg.type matching none of the valid types.
            r = -1;
            break;

    }

    return r;
}