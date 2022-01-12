#include <stdlib.h>
#include <ncurses.h>

#define WINDOW_SIZE 20
#define PADLE_SIZE 2
#define MAX_CLIENTS 10

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

void new_paddle (paddle_position_t * paddles, int legth, int id, ball_position_t ball);

void draw_paddle(WINDOW *win, paddle_position_t paddle, int ch);

void move_paddle (paddle_position_t * paddle, int direction, ball_position_t * ball, int id);

void place_ball_random(ball_position_t * ball);

void move_ball(ball_position_t * ball, paddle_position_t *paddles, int *score);

void draw_ball(WINDOW *win, ball_position_t * ball, int draw);

void copy_ball(ball_position_t *a, ball_position_t *b);

void copy_paddles(paddle_position_t * a, paddle_position_t * b);