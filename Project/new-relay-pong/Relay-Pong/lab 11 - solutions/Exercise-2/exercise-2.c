
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>

#include <pthread.h>

#define WINDOW_SIZE 15
typedef enum direction_t {UP, DOWN, LEFT, RIGHT} direction_t;

direction_t random_direction(){
    return  random()%4;

}
void new_position(int* x, int *y, direction_t direction){
        switch (direction)
        {
        case UP:
            (*x) --;
            if(*x ==0)
                *x = 2;
            break;
        case DOWN:
            (*x) ++;
            if(*x ==WINDOW_SIZE-1)
                *x = WINDOW_SIZE-3;
            break;
        case LEFT:
            (*y) --;
            if(*y ==0)
                *y = 2;
            break;
        case RIGHT:
            (*y) ++;
            if(*y ==WINDOW_SIZE-1)
                *y = WINDOW_SIZE-3;
            break;
        default:
            break;
        }
}

#define N_THREADS 6

WINDOW * big_win;
WINDOW * small_win;


pthread_mutex_t mux_curses;
void safe_waddch(WINDOW * win, int pos_x, int pos_y, int ch){

    pthread_mutex_lock(&mux_curses);    
        wmove(win, pos_y, pos_x);
        waddch(win,ch);   
        wrefresh(win);
    pthread_mutex_unlock(&mux_curses);    

}

void * move_ball_thread(void * arg){

    int ch = (char) arg;

    /* place the curso in the middle of the window */
    int pos_x = WINDOW_SIZE/2;
    int pos_y = WINDOW_SIZE/2;

    int sleep_delay;
    direction_t  direction;
    int n = 0;
    while (1)
    {
        //sleep_delay = random()%700000;
        //usleep(sleep_delay);

        sleep(1);
        /*deletes old place */
        safe_waddch(big_win, pos_x, pos_y, ' ');

        /* claculates new direction */
        direction = random_direction();

        /* claculates new mark position */
        new_position(&pos_x, &pos_y, direction);

        
        /* draw mark on new position */
        safe_waddch(big_win, pos_x, pos_y, ch| A_BOLD);
			
    }
   

}
int main()
{	
	initscr();		    	/* Start curses mode 		*/
	cbreak();				/* Line buffering disabled	*/
    keypad(stdscr, TRUE);   /* We get F1, F2 etc..		*/
	noecho();			    /* Don't echo() while we do getch */


    pthread_mutex_init(&mux_curses, NULL);
    /* creates a window and draws a border */
    big_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(big_win, 0 , 0);	
	wrefresh(big_win);
    small_win = newwin(WINDOW_SIZE/2, WINDOW_SIZE, WINDOW_SIZE, 0);
    box(small_win, 0 , 0);	
	wrefresh(small_win);


    pthread_t t_id[4];
    for(int i = 0; i < N_THREADS; i++){
        pthread_create(&t_id[i], NULL, move_ball_thread, (void *) 'a'+i);
    }

    pthread_exit(NULL);
 
  	endwin();			/* End curses mode		  */

	return 0;
}