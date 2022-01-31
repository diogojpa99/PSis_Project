#define _GNU_SOURCE
#include <pthread.h>
#include<stdio.h>
#include <stdlib.h>

#define LENGTH_ARRAY 100000
int rand_num_array[LENGTH_ARRAY], prime_array[LENGTH_ARRAY];
int global_primes = 0, global_not_primes = 0, global_index = 0;
pthread_mutex_t mutex_primes, mutex_not_primes, mutex_index;

#define N_THREADS 20
/**
 * @brief funcao que rverifica se um n;umero é primo
 * 
 * @param value valor a verificar se é primo
 * @return int 1 se value for primo 0, se não for primo
 */
int verify_prime( int value){

    int divisor = 2;
    if(value < 4){
        return 1;
    }
    while (value%divisor != 0){
        divisor ++;
    }
    if (value == divisor){
        return 1;
    }else{
        return 0;
    }
}
/**
 * @brief código da thread. itera sobre o array rand_num_array e verifica se os numero são primos
 * 
 * @param arg indica da thread. É usado para cada threda saber que valores ler do array
 * @return void* numero de primos encontrados
 */
void * inc_thread(void * arg){

    int i = 0;
    int n_primes = 0;
    while( global_index<LENGTH_ARRAY ){
        pthread_mutex_lock(&mutex_index);
        i = global_index;
        global_index++;
        pthread_mutex_unlock(&mutex_index);
        if(verify_prime(rand_num_array[i]) == 1){
            printf("%d - %lu is prime\n", i,rand_num_array[i]);
            n_primes ++;
            pthread_mutex_lock(&mutex_primes);
            global_primes++;
            prime_array[i]=rand_num_array[i];
            pthread_mutex_unlock(&mutex_primes);
        }else{
            //printf("\t\t%d %lu is not prime\n", i,rand_num_array[i]);
            pthread_mutex_lock(&mutex_not_primes);
            global_not_primes++;
            pthread_mutex_unlock(&mutex_not_primes);

        }
    } 
    printf("Thread found %d primes\n", n_primes);
    pthread_exit(n_primes);

}

/**
 * @brief programa principal que cria várias threads para verificarem se os interiso num array sao primos ou nao
 * 
 * @return int 
 */
int main(){

    pthread_mutex_init(&mutex_primes, NULL);
    pthread_mutex_init(&mutex_not_primes, NULL);
    pthread_mutex_init(&mutex_index, NULL);

    for(int  i= 0 ; i < LENGTH_ARRAY; i++){
        rand_num_array[i] = rand();
        //rand_num_array[i] = i;
    }

    pthread_t t_id[N_THREADS];
    for(int i = 0 ; i < N_THREADS; i++){
        pthread_create(&t_id[i], NULL, inc_thread, NULL);
    }


    int ret_val;
    int total_primes = 0;
    for(int i = 0 ; i < N_THREADS; i++){
        pthread_join(t_id[i], &ret_val);
        printf("Main - Thread %d - %lu found %d primes\n", i, t_id[i], ret_val);
        total_primes += ret_val;
    }

    pthread_mutex_destroy(&mutex_primes);
    pthread_mutex_destroy(&mutex_not_primes);
    pthread_mutex_destroy(&mutex_index);

    printf("main - total primes %i\n", total_primes);
    printf("main - global primes = %d, global not primes = %d\n", global_primes, global_not_primes);
    exit(0);
}