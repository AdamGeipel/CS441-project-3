/*
 * 
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
int rep_count = 0; //count num of republicans waiting for a booth
int dem_count = 0; //count num of democrats waiting for a booth
int num_republicans = 0;
int num_democrats = 0;
int num_independents = 0;
int num_booths = 0;
int total_voting = 0; //total people voting today (set in main)
int total_finished = 0; //total num people finished voting and left polling station
int total_entered = 0; //toal num entered the polling station
int *buffer = NULL;
int total_waiting = 0; //count for the initial print waiting for station to open
pthread_t *rep_threads;
pthread_t *dem_threads;
pthread_t *ind_threads;
semaphore_t mutex_rope, mutex_lineup, barrier, printing_mutex, can_vote;
semaphore_t waiting_rep, waiting_dem, waiting_ind;


/*****************************
 * Structures
 *****************************/


/*****************************
 * Global Variables
 *****************************/

/*****************************
 * Function Declarations
 *****************************/
 void crate_and_join_threads();
 
 void *republican(void *threadid);
 
 void *democrat(void *threadid);
 
 void *independent(void *threadid);

