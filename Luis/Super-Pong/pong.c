#include <stdlib.h>
#include <ncurses.h>

#include "pong.h"

#define WINDOW_SIZE 20
#define PADLE_SIZE 2
#define MAX_CLIENTS 10

//WINDOW * message_win;

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

void draw_paddle(WINDOW *win, paddle_position_t * paddle, int ch){
    
    //int ch;
    int start_x = paddle->x - paddle->length;
    int end_x = paddle->x + paddle->length;


    /*
    if(_delete){
        ch = '=';
    }else{
        ch = ' ';
    }
    */

    for (int x = start_x; x <= end_x; x++){
        wmove(win, paddle->y, x);
        waddch(win,ch);
    }
    wrefresh(win);
}

void move_paddle (paddle_position_t * paddle, int direction, ball_position_t * ball){
    int next_x = paddle->x, next_y = paddle->y;

    if (direction == KEY_UP){
        if (paddle->y  != 1){
            next_y --;
        }
    }
    if (direction == KEY_DOWN){
        if (paddle->y  != WINDOW_SIZE-2){
            next_y ++;
        }
    }
    

    if (direction == KEY_LEFT){
        if (paddle->x - paddle->length != 1){
            next_x --;
        }  
    }
    if (direction == KEY_RIGHT){
        if (paddle->x + paddle->length != WINDOW_SIZE-2){
            next_x ++;
        }
    }


    if(next_y != ball->y || ball->x < next_x - paddle->length || ball->x > next_x + paddle->length){
        // Update paddle position
        paddle->x = next_x;
        paddle->y = next_y;
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
        return;
    }

    // CHECK
    if( next_y == paddle.y && ( next_x == paddle_start_x || next_x == paddle_end_x ) ){
        ball->up_hor_down *= -1;
        ball->left_ver_right *= -1;
        return;
    }

    if( next_x == 0 || next_x == WINDOW_SIZE-1){
        ball->up_hor_down = rand() % 3 -1 ;
        ball->left_ver_right *= -1;
    }else{
        ball->x = next_x;
    }
    
    if( next_y == 0 || next_y == WINDOW_SIZE-1){
        ball->up_hor_down *= -1;
        ball->left_ver_right = rand() % 3 -1;
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

void copy_ball(ball_position_t *a, ball_position_t *b){
    a->x = b->x;
    a->y = b->y;
    a->up_hor_down = b->up_hor_down;
    a->left_ver_right = b->left_ver_right;
    a->c = b->c;
}

void copy_paddle(paddle_position_t *a, paddle_position_t *b){
    a->x = b->x;
    a->y = b->y;
    a->length = b->length;
}

/*
paddle_position_t paddle;
ball_position_t ball;
*/