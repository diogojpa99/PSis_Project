#include "pong.h"

#define SOCK_PORT 6000

typedef enum msg_type{conn, paddle_move, board_update, disconn} msg_type;

/*
    type corresponds to the type of the message.
    paddle_pos is an array containing the positions of all the paddles.
    ball_pos is the position of the ball.
    score is an array containing all the players' scores.
    id helps identify the client in question. By using it, clients can identify their own paddle and score.
    key represents the key pressed by the user.
*/
typedef struct message_t{
    msg_type type;
    paddle_position_t paddle_pos[MAX_CLIENTS];
    ball_position_t ball_pos;
    int score[MAX_CLIENTS];
    int id;
    int key;
} message_t;

/*
    Type client is used by the server to store information about the active players.
    addr and port keep the IP address and port of that client.
    id is the number attributed to that client by the server.
*/
typedef struct client{
    char addr[12];
    int port;
    int id;
    struct client *next;
}client;

client *add_new_client(client *list, char *addr, int port, int * score, paddle_position_t * paddles, int *id);

