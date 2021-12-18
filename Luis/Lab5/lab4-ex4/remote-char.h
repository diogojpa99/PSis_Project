// TODO_1 
// declaration the struct corresponding to the exchanged messages

//connection & movement

typedef enum direction_t {UP, DOWN, LEFT, RIGHT} direction_t;

typedef struct message{
    int msg_type; // 0-> connection; 1-> movement; 2-> screen refresh; 3-> remote-display-connection;
    int c; 
    direction_t direction;
    int x;
    int y;   
} message;