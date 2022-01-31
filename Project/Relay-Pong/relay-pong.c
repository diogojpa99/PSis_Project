#include "stdlib.h"
#include "string.h"

#include "relay-pong.h"

/*
    This function adds a new entry to the server's list of clients.
*/
client *add_new_client( client *list, char *addr, int port){
    client *new, *p=list;

    new = (client*) malloc(sizeof(client));
    if( new == NULL ){
        return list; // error
    }

    // Store the new client's data
    strcpy(new->addr, addr);
    new->port = port;
    new->next = NULL;

    // Insert new client at the end of the list.
    if(list==NULL)
        return new;

    while( p->next != NULL ){
        p=p->next;
    }
    p->next = new;
    return list;

}

/*
    This function is used to check the validity of a message.
    Its return is 0, for a valid msg, or -1, for an invalid one.
*/
int check_message(message_t msg){
    switch(msg.type){
        case conn:
            break;
        case rls_ball:
            if (msg.ball_pos.x < 1 || msg.ball_pos.x > WINDOW_SIZE-2 || msg.ball_pos.y < 1 || msg.ball_pos.y > WINDOW_SIZE-2 || msg.ball_pos.c != 'o')
                return -1;
            break;
        case snd_ball:
            if (msg.ball_pos.x < 1 || msg.ball_pos.x > WINDOW_SIZE-2 || msg.ball_pos.y < 1 || msg.ball_pos.y > WINDOW_SIZE-2 || msg.ball_pos.c != 'o')
                return -1;
            break;
        case mv_ball:
            if (msg.ball_pos.x < 1 || msg.ball_pos.x > WINDOW_SIZE-2 || msg.ball_pos.y < 1 || msg.ball_pos.y > WINDOW_SIZE-2 || msg.ball_pos.c != 'o')
                return -1;
            break;
        case disconn:
            break;
        default:
            return -1;
    }
    return 0;
}