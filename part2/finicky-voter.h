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

/*	 Declare semaphores	 */

// Protect counting variables, such as who is in line 
semaphore_t protect_count, 

// Control when voter can get in line
mutex_lineup, 

// Control ability to enter the voting station
barrier, 

// Protect printing operations
printing_mutex, 

// Used when checking if all voters are in the polling station
can_vote,

// Protect the number of free booths and voting activities
free_booths,

// Wait for democrats to exit
reps_wait_on_dems, 

// Wait for republicans exit 
dem_wait_on_reps;

// Protects the "in" count
semaphore_t mutex_in;

// For independents waiting to jump in line
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
