#include "pong.h"
#include <pthread.h>

#define SOCK_PORT 5000 // server port

typedef enum msg_type{conn, rls_ball, snd_ball, mv_ball, disconn} msg_type;

typedef enum state_t{wait, play} state_t;

typedef struct message_t{
    msg_type type;
    ball_position_t ball_pos;
} message_t;

typedef struct client{
    char addr[16]; // string containing the client's IP address
    int port; // local type -> use htons and ntohs
    struct client *next;
} client;

client *active_player;
pthread_mutex_t mx_client_list, mx_t_last_snd, mx_t_curr, mx_active_player;

client *add_new_client( client *list, char *addr, int port);

client *remove_client( client *list, char *addr, int port );

client *next_player(client *list);

int check_message(message_t msg);