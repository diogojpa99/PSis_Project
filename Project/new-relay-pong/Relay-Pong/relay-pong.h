#include "pong.h"

#define SOCK_PORT 5000 // server port

typedef enum msg_type{conn, rls_ball, snd_ball, mv_ball, disconn} msg_type;

typedef enum state{wait, play} state;

typedef struct message_t{
    msg_type type;
    ball_position_t ball_pos;
} message_t;

typedef struct client{
    char addr[12]; // string containing the client's IP address
    int port; // local type -> use htons and ntohs
    struct client *next;
} client;

client *add_new_client( client *list, char *addr, int port);

int check_message(message_t msg);