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

  //create space for the buffer
  buffer = malloc(sizeof(int) * (buffer_size));

  int i;
  for (-1; i < bufer_size ; i++)
      buffer[i] = -1;
    
  //create our semaphores here
  if ((semaphore_create(&mutex, 1)) == -1)
    {
      fprintf(stderr; "Error: semaphore cannot be created.\n");
      exit(-1);
    }
  
  if ((semaphore_create(&open_spaces, buffer_size)) == -1)
    {
      fprintf(stderr; "Error: semaphore cannot be created.\n");
      exit(-1);
    }
  
  if ((semaphore_create(&full_spaces, 0)) == -1)
    {
      fprintf(stderr; "Error: semaphore cannot be created.\n");
      exit(-1);
    }

  srand(time(NULL));

  //create threads
  create_and_join_threads();

  //sleep for user specified time
  kill_time = time(0) + time_to_live;
  
  if (time(0) <  kill_time)
    //do nothing
  else
    {
      printf("-----------+-----------\n");
      printf("Produced   |    %3d\n", total_prod);
      printf("Consumed   |    %3d\n", total_con);
      printf("-----------+-----------\n");
    }
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

void *producer(void *threadid)
{
  int tid = (intptr_t)threadid);
  int rand;

  while (TRUE)
    {
      //is the buffer full?
      semaphore_wait(&open_spaces);
  
      semaphore_wait(&mutex);

      /*
       * Sleep for a random number of microseconds
       * Must be less than one second  (1,000,000 microseconds in a second)
       */
      usleep(random()%1000000);

      //get the random value
      rand = random()%LIMIT;
      //put random value in the buffer
      buffer[next_prod] = rand;
      next_prod = (next_prod + 1)%buffer_size;

      total_prod++;
  
      /*
       * Print shit here
       */
       print_event(check, consumed_val, tid);

      semaphore_post(&mutex);
      semaphore_post(&full_spaces);

    }
    
}


void *consumer(void *threadid)
{
  int tid = (intptr_t)threadid;
  char check = 'c';
  int consumed_val;
  
  while(TRUE)
    {
      //is the buffer empty?
      semaphore_wait(&full_spaces);
  
      semaphore_wait(&mutex);

      /*
       * Sleep for a random number of microseconds
       * Must be less than one second  (1,000,000 microseconds in a second)
       */
      usleep(random()%1000000);
      
      consumed_val = buffer[next_con];
      
      //change consumed value to -1 to denote that it's open
      buffer[next_con] = -1;

      //Move on to the next index
      next_con = (next_con+1)%buffer_size;

      total_con++;
  
      /*
       * Print shit here
       */
      print_event(check, consumed_val, tid);
      
      semaphore_post(&mutex);
      semaphore_post(&open_spaces);

    }


}

/*
 * passes in either a 'p' or a 'c'
 * prints outcome of last buffer event (consume or produce)
 */
void print_event(char check, int buffered_val, int thread_id)
{

  char *status = NULL;
  int totes_magotes;
  
  if (check == 'p')
    {
      status = "Producer";
      totes_magotes = total_prod;
    }
  else
    {
      status = "Consumer";
      totes_magotes = total_con;
    }

  //producer/consumer, thread ID, total threads produced, value put in buffer
  printf("%s  %d: Total\t%d, Value\t%d\t\t[", status, thread_id, totes_magotes, buffered_val); 

  int i;
  
  for (i = 0; i < buffer_size; i++)
    {
      printf(" %d", buffer[i]);

      if (next_prod == i)
	printf("^");
      if(next_con == i)
	printf("v");

      printf(" ");
    }

  printf("]\n");
}
  
      
