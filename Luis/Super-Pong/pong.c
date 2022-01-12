#include <stdlib.h>
#include <ncurses.h>

#include "pong.h"

/*
    Creates a new paddle for the client with the given id.
    While creating a new paddle, it is important that it doesn't land on top of an existing one or on the ball.
    Since there are more lines than clients, we just make sure it doesn't land on the same line as another paddle.
*/
void new_paddle (paddle_position_t * paddles, int legth, int id, ball_position_t ball){
    paddles[id].x = WINDOW_SIZE/2;
    paddles[id].y = WINDOW_SIZE-2;
    paddles[id].length = legth;
    int i=0;
    // Test for collisions.
    while(i<MAX_CLIENTS){
        // If the new paddle is in the same line as another one
        if( i!=id && paddles[id].y==paddles[i].y ){
            // Move it up a line and start checking for collisions again.
            paddles[id].y--;
            i=0;
        }
        else
            i++; // If not, check the next paddle.
        // Lastly, check for collisions with the ball.
        if(i==MAX_CLIENTS){
             // If the new paddle is in the same line as the ball
            if(paddles[id].y == ball.y){
                // Move it up a line and start checking for collisions again.
                paddles[id].y--;
                i=0;
            }
        }
    }
    // When this loop ends, it means we have found a valid position for the new paddle.
    return;
}

void draw_paddle(WINDOW *win, paddle_position_t paddle, int ch){
    int start_x = paddle.x - paddle.length;
    int end_x = paddle.x + paddle.length;

    if(paddle.x <= 0 || paddle.y <= 0)
        return;

    for (int x = start_x; x <= end_x; x++){
        wmove(win, paddle.y, x);
        waddch(win,ch);
    }
    wrefresh(win);
}

/*
    This function moves the paddle of the player with the given id.
    It avoids collisions with the ball and with other paddles.
*/
void move_paddle (paddle_position_t * paddles, int direction, ball_position_t * ball, int id){
    int next_x = paddles[id].x, next_y = paddles[id].y;

    if (direction == KEY_UP){
        if (paddles[id].y  != 1){
            next_y --;
        }
    }
    if (direction == KEY_DOWN){
        if (paddles[id].y  != WINDOW_SIZE-2){
            next_y ++;
        }
    }
    if (direction == KEY_LEFT){
        if (paddles[id].x - paddles[id].length != 1){
            next_x --;
        }  
    }
    if (direction == KEY_RIGHT){
        if (paddles[id].x + paddles[id].length != WINDOW_SIZE-2){
            next_x ++;
        }
    }

    // If there is a collision with any other paddle, our paddle doesn't move.
    for(int i=0; i<MAX_CLIENTS; i++){
        if(i != id && next_y == paddles[i].y && next_x+PADLE_SIZE >= paddles[i].x-PADLE_SIZE && next_x-PADLE_SIZE <= paddles[i].x+PADLE_SIZE)
            return;
    }
    // If there is no collision with the ball, the paddle moves.
    if(next_y != ball->y || ball->x < next_x - PADLE_SIZE || ball->x > next_x + PADLE_SIZE){
        // Update paddle position
        paddles[id].x = next_x;
        paddles[id].y = next_y;
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

/*
    This function moves the ball.

*/
void move_ball(ball_position_t * ball, paddle_position_t *paddles, int *score){
    
    int next_x = ball->x + ball->left_ver_right;
    int next_y = ball->y + ball->up_hor_down;

    int paddle_start_x;
    int paddle_end_x;

    for(int i=0; i<MAX_CLIENTS; i++){
        paddle_start_x = paddles[i].x - PADLE_SIZE;
        paddle_end_x = paddles[i].x + PADLE_SIZE;

        if( next_y == paddles[i].y && next_x > paddle_start_x && next_x < paddle_end_x ){
            ball->up_hor_down *= -1;
            ball->left_ver_right = rand() % 3 -1;
            score[i]++;
            return;
        }
        if( next_y == paddles[i].y && ( next_x == paddle_start_x || next_x == paddle_end_x ) ){
            ball->up_hor_down *= -1;
            ball->left_ver_right *= -1;
            score[i]++;
            return;
        }
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

void copy_paddles(paddle_position_t * a, paddle_position_t * b){
    for(int i=0;i<MAX_CLIENTS;i++){
        a[i].x = b[i].x;
        a[i].y = b[i].y;
        a[i].length = b[i].length;
    }
}