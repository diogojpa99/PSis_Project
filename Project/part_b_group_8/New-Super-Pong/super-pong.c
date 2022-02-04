#include "super-pong.h"
#include <string.h>

/*
    The server holds a list containing information about the clients
    This function is used when a new client tries to connect to the server.
    It is also where clients get their id.
*/
client *add_new_client(client *list, char *addr, int port, int * score, paddle_position_t * paddles, int *id, ball_position_t ball, int fd){
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
    
    new->fd = fd;
    new->id = *id = i;
    new->next = list;   // Client is inserted at the head of the list.
    new_paddle(paddles, PADLE_SIZE, i, ball);
    score[i] = 0;

    return new;
}

client *remove_client(client *list, int fd, int *id){
    client *ptr1, *ptr2;

    ptr1 = list;
    if(ptr1->fd == fd){
        // first client in list
        *id = ptr1->id;
        ptr2 = ptr1->next;
        free(ptr1);
        return ptr2;
    }else{
        while(ptr1->fd != fd && ptr1 != NULL){
            ptr2 = ptr1;
            ptr1 = ptr1->next;
        }
        if(ptr1 != NULL){
            // requested client was found and will be removed
            *id = ptr1->id;
            ptr2->next = ptr1->next;
            free(ptr1);
            return list;
        }
    }
}

/*
    Each time a message is received, it must be validated.
    This function validates messages acording to their type.
*/
int check_message(message_t msg){
    int r = 0;

    switch(msg.type){
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
        default:
            // The default case is equivalent to msg.type matching none of the valid types.
            r = -1;
            break;

    }

    return r;
}