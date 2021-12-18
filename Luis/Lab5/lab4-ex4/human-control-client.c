<<<<<<< HEAD:Luis/Lab5/lab4-ex4/human-control-client.c
#include <ncurses.h>
#include <sys/un.h>
#include <sys/socket.h>
#include "remote-char.h"
#include "sock_dg.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  
#include <ctype.h> 
#include <stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h> 

int main()
{
   
    //TODO_4

    int fd;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd==-1){
        perror("socket: ");
        exit(-1);
    }

    struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SOCK_PORT);
    char linha[100];
    printf("What is the network address of the recipient? ");
	do{
        fgets(linha, 100, stdin);
        linha[strlen(linha)-1] = '\0';	
	    if( inet_pton(AF_INET, linha, &server_addr.sin_addr) < 1){
	    	printf("Not a valid address. Try again. ");
	    }
    }while(inet_pton(AF_INET, linha, &server_addr.sin_addr) < 1);

    //TODO_5
    // read the character from the user

    char c;

    printf("Insert the character you want to control: ");
    if ( (c = fgetc(stdin)) == EOF ){
        printf("Error reading character\n");
        exit(-1);
    }

    // TODO_6
    // send connection message

    //Definir a estrutura message
    message m;

    m.c = (int) c;
    m.msg_type = 0; //connection
    int nbytes;
    do{
        nbytes = sendto(fd, &m, sizeof(message), 0, (const struct sockaddr *) &server_addr, sizeof(server_addr));
        printf("%d bytes sent %c\n", nbytes, m.c);
    }while(nbytes <= 0);
    /*********************************/


	initscr();			/* Start curses mode 		*/
	cbreak();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();			/* Don't echo() while we do getch */

    
    int ch;
    int n = 0;
    direction_t direction;


    do
    {   

        mvprintw(1,0,"You can move the character");

    	ch = getch();		
        n++;
        switch (ch)
        {
            case KEY_LEFT:
                mvprintw(0,0,"%d Left arrow is pressed", n);
                direction = LEFT;
                break;
            case KEY_RIGHT:
                mvprintw(0,0,"%d Right arrow is pressed", n);
                direction = RIGHT;
                break;
            case KEY_DOWN:
                mvprintw(0,0,"%d Down arrow is pressed", n);
                direction = DOWN;
                break;
            case KEY_UP:
                mvprintw(0,0,"%d :Up arrow is pressed", n);
                direction = UP;
                break;
            default:
                ch = 'x';
                    break;
        }
        refresh();			/* Print it on to the real screen */
        
        
        //TODO_9
        // prepare the movement message
        m.c = c;
        m.msg_type = 1; // Movement
        m.direction = direction;

        //TODO_10
        //send the movement message
        nbytes = sendto(fd, &m, sizeof(message), 0, (const struct sockaddr *) &server_addr, sizeof(server_addr));

        /********************************/

        //receive reply from server
        char recv_msg[10];
        nbytes = recv(fd, recv_msg, 10, 0);
        mvprintw(2,0,"%s", recv_msg);
        if(strcmp(recv_msg, "HIT") == 0)
            flash();

    }while(ch != 27);
    
    
  	endwin();			/* End curses mode		  */

    close(fd);

	return 0;
=======
#include <ncurses.h>
#include <sys/un.h>
#include <sys/socket.h>
#include "remote-char.h"
#include "sock_dg.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  
#include <ctype.h> 
#include <stdlib.h>
 

int main()
{
   
    //TODO_4

    int fd;

    fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(fd==-1){
        perror("socket: ");
        exit(-1);
    }

    struct sockaddr_un server_addr, local_addr;
	server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SOCK_ADDRESS);
    local_addr.sun_family = AF_UNIX;
	sprintf(local_addr.sun_path, "%s %d", SOCK_ADDRESS, getpid());

    int err = bind(fd, (struct sockaddr *) &local_addr, sizeof(local_addr));
    if(err==-1){
        perror("bind");
        exit(-1);
    } 

    //TODO_5
    // read the character from the user

    char c;

    printf("Insert the character you want to control: ");
    if ( (c = fgetc(stdin)) == EOF ){
        printf("Error reading character\n");
        exit(-1);
    }

    // TODO_6
    // send connection message

    //Definir a estrutura message
    message m;

    m.c = (int) c;
    m.msg_type = 0; //connection
    int nbytes;

    nbytes = sendto(fd, &m, sizeof(message), 0, (const struct sockaddr *) &server_addr, sizeof(server_addr));
    printf("%d bytes sent %c\n", nbytes, m.c);
    
    /*********************************/


	initscr();			/* Start curses mode 		*/
	cbreak();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();			/* Don't echo() while we do getch */

    
    int ch;
    int n = 0;
    direction_t direction;


    do
    {   

        mvprintw(1,0,"You can move the character");

    	ch = getch();		
        n++;
        switch (ch)
        {
            case KEY_LEFT:
                mvprintw(0,0,"%d Left arrow is pressed", n);
                direction = LEFT;
                break;
            case KEY_RIGHT:
                mvprintw(0,0,"%d Right arrow is pressed", n);
                direction = RIGHT;
                break;
            case KEY_DOWN:
                mvprintw(0,0,"%d Down arrow is pressed", n);
                direction = DOWN;
                break;
            case KEY_UP:
                mvprintw(0,0,"%d :Up arrow is pressed", n);
                direction = UP;
                break;
            default:
                ch = 'x';
                    break;
        }
        refresh();			/* Print it on to the real screen */
        
        
        //TODO_9
        // prepare the movement message
        m.c = c;
        m.msg_type = 1; // Movement
        m.direction = direction;

        //TODO_10
        //send the movement message
        nbytes = sendto(fd, &m, sizeof(message), 0, (const struct sockaddr *) &server_addr, sizeof(server_addr));

        /********************************/

        //receive reply from server
        char recv_msg[10];
        nbytes = recv(fd, recv_msg, 10, 0);
        if(strcmp(recv_msg, "HIT") == 0)
            flash();

    }while(ch != 27);
    
    
  	endwin();			/* End curses mode		  */

    close(fd);

	return 0;
>>>>>>> e0503c60f96ee620895f46912b1b520b39c50ded:Luis/Lab4/Ex4/human-control-client.c
}