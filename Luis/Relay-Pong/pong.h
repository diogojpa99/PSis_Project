#include <stdlib.h>
#include <ncurses.h>

#define WINDOW_SIZE 20
#define PADLE_SIZE 2
#define MAX_CLIENTS 10

WINDOW * message_win;

//enum direction_t{up, down, left, right};

typedef struct ball_position_t{
    int x, y;
    int up_hor_down; //  -1 up, 0 horizontal, 1 down
    int left_ver_right; //  -1 left, 0 vertical,1 right
    char c;
} ball_position_t;

typedef struct paddle_position_t{
    int x, y;
    int length;
} paddle_position_t;

void new_paddle (paddle_position_t * paddle, int legth);

void draw_paddle(WINDOW *win, paddle_position_t * paddle, int _delete);

void moove_paddle (paddle_position_t * paddle, int direction);

void place_ball_random(ball_position_t * ball);

void move_ball(ball_position_t * ball, paddle_position_t paddle);

void draw_ball(WINDOW *win, ball_position_t * ball, int draw);

paddle_position_t paddle;
ball_position_t ball;