/*
 * 
 * CS 441/541: Finicky Voter (Project 3 Part 2)
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

/*****************************
 * Structures
 *****************************/


/*****************************
 * Global Variables
 *****************************/
//num of republicans waiting in line for a booth
int rep_inline = 0; 
//num of democrats waiting in line for a booth
int dem_inline = 0; 

//Total number of voters given by argument
int num_republicans = 0;
int num_democrats = 0;
int num_independents = 0;
int num_booths = 0;

int total_voting = 0; //total people voting today (set in main)
int total_finished = 0; //total num people finished voting and left polling station
int total_entered = 0; //toal num entered the polling station
int *buffer = NULL; //Array that is printed to show booth status
int total_waiting = 0; //count for the initial print waiting for station to open
int num_inline = 0; //keeps track of the number of people in line/voting

pthread_t *rep_threads;
pthread_t *dem_threads;
pthread_t *ind_threads;
semaphore_t protect_count, mutex_lineup, barrier, printing_mutex, can_vote;
semaphore_t free_booths;

semaphore_t reps_wait_on_dems, dem_wait_on_reps;

//protects the "in" count
semaphore_t mutex_in;

//for independents waiting to jump in line
semaphore_t inds_waiting_for_line;

/*****************************
 * Function Declarations
 *****************************/
void create_and_join_threads();
 
void *republicans(void *threadid);
 
void *democrats(void *threadid);
 
void *independents(void *threadid);
 
/*
 * prints a change in state: entering, waiting, leaving
 */
void print_voting_status(char party, int threadid, char status );
 
/*
 * prints when a thread is "voting!"
 */
void print_voting( char party, int threadid, int booth );
 
/*
 * prints when a thread is waiting to enter the polling station
 */
void print_wait_polling_station( char party, int threadid);

void create_and_join_threads();
