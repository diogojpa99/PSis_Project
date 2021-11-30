#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h> //new

int main(){
    char line[100];
    int n;

    time_t t;//new

    srand((unsigned) time(&t));//new

    printf("What is your student number");
    fgets(line, 100, stdin);

    if (sscanf(line, "%d", &n) != 1){
        printf("invalid Number\n");
        exit(-1);
    }
    //srandom(n);
    printf("your own random %ld\n", random());   
    exit(0);
}