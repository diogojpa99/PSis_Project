#include "pong.h"

#define SOCK_PORT 6000

enum msg_type{connection, paddle_move, board_update, disconnect};

typedef struct message_t{
    msg_type type;
    paddle_position_t paddle_pos;
    ball_position_t ball_pos;
    int score[MAX_CLIENTS];
    int id;
    direction_t direction;
} message_t;