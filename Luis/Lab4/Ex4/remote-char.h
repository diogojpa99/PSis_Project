// TODO_1 
// declaration the struct corresponding to the exchanged messages

//connection & movement

typedef enum direction_t {UP, DOWN, LEFT, RIGHT} direction_t;

typedef struct message{
    int msg_type;
    int c; 
    direction_t direction;   
} message;