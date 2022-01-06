#include "super-pong.h"
#include <string.h>

client *add_new_client(client *list, char *addr, int port, int * score, paddle_position_t * paddles, int *id){
    client *new, *p=list;
    int i = 0;

    new = (client*) malloc(sizeof(client));
    if( new == NULL ){
        return list; // error
    }

    // Store the new client's data
    while(i<MAX_CLIENTS && score[i]!=-1){
        i++;
    }
    strcpy(new->addr, addr);
    new->port = port;
    new->id = *id = i;
    new->next = list;

    new_paddle(paddles, PADLE_SIZE, i);
    score[i] = 0;

    return new;
    
    // Insert new client at the end of the list.
    /*
    if(list==NULL)
        return new;

    while( p->next != NULL ){
        p=p->next;
    }
    p->next = new;
    return list;
    */
}