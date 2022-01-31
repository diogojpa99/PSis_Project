#include "stdlib.h"
#include "string.h"
#include <pthread.h>
#include "relay-pong.h"

client *active_player;
pthread_mutex_t mx_client_list, mx_t_last_snd, mx_t_curr, mx_active_player;

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
    This function removes a client from the list
*/
client *remove_client( client *list, char *addr, int port ){
    client *ptr1=list, *ptr2;

    // If the client we need to remove is the first in the list
    if( strcmp( list->addr, addr)==0 && list->port == port ){
        // Free client and return the new head
        ptr1 = list->next;
        free(list);
        return ptr1;
    }
    else{
        // Search the list
        while( ptr1!=NULL && ( strcmp(ptr1->addr, addr)!=0 || ptr1->port != port ) ){
            ptr2=ptr1;
            ptr1=ptr1->next;
        }
        if(ptr1 != NULL){
            ptr2->next = ptr1->next;
            free(ptr1);
            return list;
        }
        else
            return list;
    }
}

/*
    This function chooses the next active player.
*/
client *next_player(client *list){
    if(active_player == NULL){
        return list;
    }
    if(active_player->next == NULL)
        return list;
    else
        return active_player->next;
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