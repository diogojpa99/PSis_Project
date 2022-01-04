#include "super-pong.h"
#include <string.h>

client *add_new_client(client *list, char *addr, int port, int * score, paddle_position_t * paddles){
    client *new, *p=list;
    int id=0;

    new = (client*) malloc(sizeof(client));
    if( new == NULL ){
        return list; // error
    }

    // Store the new client's data
    while(id<MAX_CLIENTS && score[id]!=-1){
        id++;
    }
    strcpy(new->addr, addr);
    new->port = port;
    new->id = id;
    new->next = NULL;

    new_paddle(paddles, PADLE_SIZE, id);

    // Insert new client at the end of the list.
    if(list==NULL)
        return new;

    while( p->next != NULL ){
        p=p->next;
    }
    p->next = new;
    return list;

}