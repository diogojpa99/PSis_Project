#include "pong.h"

#define SOCK_PORT 6000

typedef enum msg_type{conn, paddle_move, board_update, disconn} msg_type;

typedef struct message_t{
    msg_type type;
    paddle_position_t paddle_pos[MAX_CLIENTS];
    ball_position_t ball_pos;
    int score[MAX_CLIENTS];
    int id;
    int key;
} message_t;