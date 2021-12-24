#include <stdlib.h>
#include <ncurses.h>

#include "pong.h"

#define WINDOW_SIZE 20
#define PADLE_SIZE 2
#define MAX_CLIENTS 10

WINDOW * message_win;

enum direction_t{up, down, left, right};

/*
typedef struct client{
    char addr[12]; // string containing the client's IP address
    int port; // local type -> use htons and ntohs
    struct client *next;
} client;

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
*/

void new_paddle (paddle_position_t * paddle, int legth){
    paddle->x = WINDOW_SIZE/2;
    paddle->y = WINDOW_SIZE-2;
    paddle->length = legth;
}

void draw_paddle(WINDOW *win, paddle_position_t * paddle, int _delete){
    
    int ch;
    int start_x = paddle->x - paddle->length;
    int end_x = paddle->x + paddle->length;


    if(_delete){
        ch = '=';
    }else{
        ch = ' ';
    }

    for (int x = start_x; x <= end_x; x++){
        wmove(win, paddle->y, x);
        waddch(win,ch);
    }
    wrefresh(win);
}

void moove_paddle (paddle_position_t * paddle, int direction){
    if (direction == KEY_UP){
        if (paddle->y  != 1){
            paddle->y --;
        }
    }
    if (direction == KEY_DOWN){
        if (paddle->y  != WINDOW_SIZE-2){
            paddle->y ++;
        }
    }
    

    if (direction == KEY_LEFT){
        if (paddle->x - paddle->length != 1){
            paddle->x --;
        }
    }
    if (direction == KEY_RIGHT)
        if (paddle->x + paddle->length != WINDOW_SIZE-2){
            paddle->x ++;
    }

    return;
}

void place_ball_random(ball_position_t * ball){
    ball->x = rand() % WINDOW_SIZE ;
    ball->y = rand() % WINDOW_SIZE ;
    ball->c = 'o';
    ball->up_hor_down = rand() % 3 -1; //  -1 up, 1 - down
    ball->left_ver_right = rand() % 3 -1 ; // 0 vertical, -1 left, 1 right
}


// Acrescentou-se paddle como argumrnto
// Interceção da bola com paddle feita na função moove_ball
void move_ball(ball_position_t * ball, paddle_position_t paddle){
    
    int next_x = ball->x + ball->left_ver_right;
    int next_y = ball->y + ball->up_hor_down;

    int paddle_start_x = paddle.x - paddle.length;
    int paddle_end_x = paddle.x + paddle.length;


    //New
    if( next_y == paddle.y && next_x > paddle_start_x && next_x < paddle_end_x ){
        ball->up_hor_down *= -1;
        ball->left_ver_right = rand() % 3 -1;
        mvwprintw(message_win, 2,1,"bottom top win");
        wrefresh(message_win);
        return;
    }

    // CHECK
    if( next_y == paddle.y && ( next_x == paddle_start_x || next_x == paddle_end_x ) ){
        ball->up_hor_down *= -1;
        ball->left_ver_right *= -1;
        mvwprintw(message_win, 2,1,"bottom top win");
        wrefresh(message_win);
        return;
    }

    if( next_x == 0 || next_x == WINDOW_SIZE-1){
        ball->up_hor_down = rand() % 3 -1 ;
        ball->left_ver_right *= -1;
        mvwprintw(message_win, 2,1,"left right win");
        wrefresh(message_win);
    }else{
        ball->x = next_x;
    }
    
    if( next_y == 0 || next_y == WINDOW_SIZE-1){
        ball->up_hor_down *= -1;
        ball->left_ver_right = rand() % 3 -1;
        mvwprintw(message_win, 2,1,"bottom top win");
        wrefresh(message_win);
    }else{
        ball -> y = next_y;
    }

    return;
}

void draw_ball(WINDOW *win, ball_position_t * ball, int draw){

    int ch;

    if(draw){
        ch = ball->c;
    }else{
        ch = ' ';
    }
    wmove(win, ball->y, ball->x);
    waddch(win,ch);
    wrefresh(win);

    return;
}

paddle_position_t paddle;
ball_position_t ball;