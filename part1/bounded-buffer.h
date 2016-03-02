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

struct buffer_val
{
  int rand;
  int is_next_consumed;
  int is_next_produced;
}
  typedef struct buffer_val buffer_val;

/*****************************
 * Global Variables
 *****************************/

int numProd = 0;
int time_to_live = 0;
int num_producers = 0;
int num_consumers = 0;
int buffer_size = 10;
pthread_t prod_threads, con_threads;
semaphore_t print, mutex, can_produce, can_consume;

/*****************************
 * Function Declarations
 *****************************/

void create_and_join_threads();

void *consumer(void *threadid);

void *producer(void *threadid);

volid read_args();
