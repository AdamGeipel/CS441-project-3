/*
 * 
 *
 * CS 441/541: Bounded Buffer (Project 3 Part 1)
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "semaphore_support.h"

/*****************************
 * Defines
 *****************************/

#define LIMIT 10
#define TRUE 1
#define FALSE 0

/*****************************
 * Structures
 *****************************/

/*****************************
 * Global Variables
 *****************************/

int numProd = 0;
int time_to_live = 0;
int num_producers = 0;
int num_consumers = 0;
int buffer_size = 10;
pthread_t prod_threads, con_threads;
semaphore_t mutex, open_spaces, full_spaces;
int next_prod = 0;
int next_con = 0;
int total_prod = 0;
int total_con = 0;
int *buffer;
int kill_time;



/*****************************
 * Function Declarations
 *****************************/

void create_and_join_threads();

void *consumer(void *threadid);

void *producer(void *threadid);

void read_args();

void print_event(char check, int buffered_val, int thread_id);
