#include "stdlib.h"
#include "string.h"

#include "relay-pong.h"

// Adds a new entry to the server's list of clients.
// Returns:
//   -1 if an error occurs during memory allocation;
//   0 if successful.
int add_new_client( client *list, char *addr, int port){
    client *new, *p=list;

    new = (client*) malloc(sizeof(client));
    if( new == NULL ){
        return -1; // error
    }

    // Store the new client's data
    strcpy(new->addr, addr);
    new->port = port;
    new->next = NULL;

    // Insert new client at the end of the list.
    while( p->next != NULL ){
        p=p->next;
    }
    p->next = new;

    return 0;
}