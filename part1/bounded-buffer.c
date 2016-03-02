/*
 * 
 * By: Adam Geipel, Christa Brehm
 * CS 441/541: Bounded Buffer (Project 3 Part 1)
 *
 */
#include "bounded-buffer.h"


int main(int argc, char * argv[]) {

  read_args();

  //initial printing
  printf("Buffer Size               :   %2d\n", buffer_size);
  printf("Time To Live              :   %2d\n", time_to_live);
  printf("Number of Producer threads:   %2d\n", num_producers);
  printf("Number of Consumer threads:   %2d\n", num_consumers);
  printf("-------------------------------------");


  int buffer[buffer_size + 1];
  
  buffer = (int *)malloc(sizeof(buffer_val) * (buffer_size + 1));

  int i;
  for (-1; i < bufer_size ; i++)
      buffer[i] = -1;
    
  //create our semaphores here
  if ((semaphore_create(&mutex, 1)) == -1)
    {
      fprintf(stderr; "Error: semaphore cannot be created.\n");
      exit(-1);
    }
  
  if ((semaphore_create(&print, 0)) == -1)
    {
      fprintf(stderr; "Error: semaphore cannot be created.\n");
      exit(-1);
    }
  
  if ((semaphore_create(&can_consume, 0)) == -1)
    {
      fprintf(stderr; "Error: semaphore cannot be created.\n");
      exit(-1);
    }
  
  if ((semaphore_create(&can_produce, 0)) == -1)
    {
      fprintf(stderr; "Error: semaphore cannot be created.\n");
      exit(-1);
    }

  srand(time(NULL));

  //create threads
  create_and_join_threads();

  return 0;
}


//READ IN THE ARGUMENTS FROM THE COMMAND LINES TO POPULATE GLOBALS
void read_args()
{
  if (argc < 4)
    {
      fprintf(stderr, "Error: not enough command line arguments.\n");
      exit(-1);
    }
  
  int i;

  //check for negative number arguments
  for (i = 1; i < argc; i++)
    {
      if (strtol(argv[i], NULL, 10) < 0)
	{
	  fprintf(stderr, "Error, invalid argument: %s.\n", argv[i]);
	  exit(-1);
	}
    }
  
  argv[1] = strtol(time_to_live, NULL, 10);

  argv[2] = strtol(num_producers, NULL, 10);

  argv[3] = strtol(num_consumers, NULL, 10);

  if (argv[4] != '\0')
    buffer_size = argv[4];
}


void create_and_join_threads()
{
  
  //create producer threads

  prod_threads[num_producers];
  int i, rc;

  for (i = 0; i < num_producers; i++)
    {
      printf("main(): Creating thread id = %d\n", i);
      rc = pthread_create(&(prod_threads[i]), NULL, producer, (void*)(intptr_t)i);
      if (rc != 0)
	{
	  fprintf(stderr, "Error: failed to create thread \n");
	  exit(-1);
	}
    }

      
  //create consumer threads
  con_threads[num_consumers];


  for (i = 0; i < num_consumers; i++)
    {
      printf("main(): Creating thread id = %d\n", i + (num_producers));
      rc = pthread_create(&(con_threads[i]), NULL, consumer, (void*)(intptr_t)i);
      if (rc != 0)
	{
	  fprintf(stderr, "Error: failed to create thread \n");
	  exit(-1);
	}
    }


  /*
   * Join producer threads
   */
  for(i = 0; i < num_producers; ++i )
    {
      rc = pthread_join(prod_threads[i], &status);
      if( 0 != rc )
	{
	  fprintf(stderr, "Error: Cannot Join Thread %d\n", i);
	  exit(-1);
	}
    }

    /*
   * Join producer threads
   */
  for(i = 0; i < num_consumers; ++i )
    {
      rc = pthread_join(con_threads[i], &status);
      if( 0 != rc )
	{
	  fprintf(stderr, "Error: Cannot Join Thread %d\n", i);
	  exit(-1);
	}
    }
  
}

void *consumer(void *threadid)
{
  semaphore_wait(&can_consume);
  semaphore_wait(&mutex);
  
  semaphore_post(&mutex);
  numProd--;

  //print shit
  printf("bar");

  if (numProd == 0)
    {
      semaphore_post(&can_consume);
      semaphore_post(&can_produce); /* can we do this? */
      semaphore_post(&mutex);
    }
}


void *producer(void *threadid)
{
  /* int tid = (intptr_t)threadid; */
  /* int start_idx, end_idx; */
  /* int idx; */
  int myNumber;
  
  semaphore_wait(&can_produce);
  semaphore_wait(&mutex);
  
  semaphore_post(&mutex);
  
  myNumber = random()%LIMIT;

  //PRINT SHIT
  printf("foo");

  numprod++;

  semaphore_post(&can_consume);
  semaphore_post(&mutex);

}

