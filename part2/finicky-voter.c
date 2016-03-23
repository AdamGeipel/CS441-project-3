/*
 *
 * CS 441/541: Finicky Voter (Project 3 Part 2)
 *
 */
#include "finicky-voter.h"

int main(int argc, char * argv[]) {
    int i;
	//check for negative number arguments
	for (i = 1; i < argc; i++){
		if (strtol(argv[i], NULL, 10) < 0){
                    fprintf(stderr, "Error, invalid argument: %s.\n", argv[i]);
		    exit(-1);
		}
	}
	
	/*
	 * read arguments
	 */
	if( argc == 1 ){ //all defaults
		num_booths = 10;
		num_republicans = 5;
		num_democrats = 5;
		num_independents = 5;
	}else if( argc == 2 ){
		num_booths = strtol(argv[1], NULL, 10);
		num_republicans = 5;
		num_democrats = 5;
		num_independents = 5;
	}else if( argc == 3 ){
		num_booths = strtol(argv[1], NULL, 10);
		num_republicans = (int)strtol(argv[2], NULL, 10);
		num_democrats = 5;
		num_independents = 5;
	}else if( argc == 4 ){
		num_booths = strtol(argv[1], NULL, 10);
		num_republicans = (int)strtol(argv[2], NULL, 10);
		num_democrats = (int)strtol(argv[3], NULL, 10);
		num_independents = 5;
	}else if( argc == 5 ){
		num_booths = strtol(argv[1], NULL, 10);
		num_republicans = (int)strtol(argv[2], NULL, 10);
		num_democrats = (int)strtol(argv[3], NULL, 10);
		num_independents = (int)strtol(argv[4], NULL, 10);
	}else{
		fprintf(stderr, "Error, too many arguments\n");
		exit(-1);
	}
	
	total_voting = (num_republicans + num_democrats + num_independents);
	printf("total voting = %d\n", total_voting);
        //initial printing
	printf("Number of Voing Booths   :   %2d\n", num_booths);
	printf("Number of Republican     :   %2d\n", num_republicans);
	printf("Number of Democrat       :   %2d\n", num_democrats);
	printf("Number of Independent    :   %2d\n", num_independents);
	printf("-----------------------+-----------------------+-----------------------\n");
  
	//create space for the buffer
	buffer = malloc(sizeof(int) * (num_booths));
	//voting booths are originally empty
	for ( i = 0; i < num_booths; i++){
		buffer[i] = '.';
	}

    //create our semaphores
    if ((semaphore_create(&mutex_rope, 0)) == -1) //maybe 1 for goahead?
    {
      fprintf(stderr, "Error: semaphore cannot be created.\n");
      exit(-1);
    }
    if ((semaphore_create(&mutex_lineup, 1)) == -1) 
    {
      fprintf(stderr, "Error: semaphore cannot be created.\n");
      exit(-1);
    }
    if ((semaphore_create(&barrier, 0)) == -1)//initially locked unitl all threads are created 
    {
      fprintf(stderr, "Error: semaphore cannot be created.\n");
      exit(-1);
    }
    if ((semaphore_create(&waiting_rep, num_republicans)) == -1)
    {
      fprintf(stderr, "Error: semaphore cannot be created.\n");
      exit(-1);
    }
    if ((semaphore_create(&waiting_dem, num_democrats)) == -1)
    {
      fprintf(stderr, "Error: semaphore cannot be created.\n");
      exit(-1);
    }
    if ((semaphore_create(&waiting_ind, num_independents)) == -1)
    {
      fprintf(stderr, "Error: semaphore cannot be created.\n");
      exit(-1);
    }
    if ((semaphore_create(&printing_mutex, 1)) == -1)//protect prints with global counts
	{
		fprintf(stderr, "Error: semaphore cannot be created.\n");
		exit(-1);
	}
    if ((semaphore_create(&can_vote, 0)) == -1)//initally can't vote until all people have entered the polling station
	{
		fprintf(stderr, "Error: semaphore cannot be created.\n");
		exit(-1);
	}
	if ((semaphore_create(&rep_barrier, 0)) == -1) 
    {
      fprintf(stderr, "Error: semaphore cannot be created.\n");
      exit(-1);
    }
	if ((semaphore_create(&dem_barrier, 0)) == -1) 
    {
      fprintf(stderr, "Error: semaphore cannot be created.\n");
      exit(-1);
    }
	if ((semaphore_create(&voting_booths, num_booths)) == -1) 
    {
      fprintf(stderr, "Error: semaphore cannot be created.\n");
      exit(-1);
    }

	srand(time(NULL)); //seed the time. Does this go here??
	
	//create threads
       create_and_join_threads();
       //sleeping and post the initial barrier is now done in create_and_join_threads method	
  
    return 0;
}

void create_and_join_threads(){
	int i, rc;
	
	 /*
	  * Create all threads
	  */
	  
	rep_threads = (pthread_t *)malloc(sizeof(pthread_t) * num_republicans);
	//create republican threads
	for(i = 0; i < num_republicans; i++){
		rc = pthread_create(&(rep_threads[i]), NULL, republican, (void*)(intptr_t)i);
		if (rc != 0){
			fprintf(stderr, "Error: failed to create thread \n");
			exit(-1);
		}
	}
	
	dem_threads = (pthread_t *)malloc(sizeof(pthread_t) * num_democrats);
	//create democrat threads
	for(i = 0; i < num_democrats; i++){
		rc = pthread_create(&(dem_threads[i]), NULL, democrat, (void*)(intptr_t)i);
		if (rc != 0){
			fprintf(stderr, "Error: failed to create thread \n");
			exit(-1);
		}
	}
	
	ind_threads = (pthread_t *)malloc(sizeof(pthread_t) * num_independents);
	//create independent threads
	for(i = 0; i < num_independents; i++){
		rc = pthread_create(&(ind_threads[i]), NULL, independent, (void*)(intptr_t)i);
		if (rc != 0){
			fprintf(stderr, "Error: failed to create thread \n");
			exit(-1);
		}
	}
        
       //all threads created so sleep for two seconds
        usleep(2000000);
        printf("done sleeping\n");
        //open up polling station
        semaphore_post(&barrier);
        printf("posting\n");

	
	/*
	 * Join all threads
	 */
	for(i = 0; i < num_republicans; i++ ){
      rc = pthread_join(rep_threads[i], NULL);
		if( 0 != rc ){
			fprintf(stderr, "Error: Cannot Join Thread %d\n", i);
			exit(-1);
		}
    }
    for(i = 0; i < num_democrats; i++ ){
        rc = pthread_join(dem_threads[i], NULL);
	if( 0 != rc ){
		fprintf(stderr, "Error: Cannot Join Thread %d\n", i);
		exit(-1);
	}
    }
    for(i = 0; i < num_independents; i++ ){
        rc = pthread_join(ind_threads[i], NULL);
	if( 0 != rc ){
            fprintf(stderr, "Error: Cannot Join Thread %d\n", i);
            exit(-1);
	}
    }
}

/*
 * only one thread can be in here because the call is wrapped in a mutex
 */
void print_wait_polling_station( char party, int threadid){
	int i;
	char *party_status = NULL;
	
	if( party == 'R' ){
		party_status = "Republican";
	}else if( party == 'D' ){
		party_status = "Democrat";
	}else{
		party_status = "Independent";
	}
	
	printf("%s    %d \t|->  ", party_status, threadid);
	for(i = 0; i < num_booths; i++){
		printf("[%c]", buffer[i]);
	}
	printf("  <-| Waiting for polling station to open...\n");
	
	total_waiting++; //waiting to enter polling station
	
	if(total_waiting == total_voting){
		printf("-----------------------+-----------------------+-----------------------\n");
	}
}

void print_voting( char party, int threadid, int booth ){
	int i;
	char *party_status = NULL;
	
	if( party == 'R' ){
		party_status = "Republican";
	}else if( party == 'D' ){
		party_status = "Democrat";
	}else{
		party_status = "Independent";
	}
	
	printf("%s    %d in   %d|->  ", party_status, threadid, booth);
	for(i = 0; i < num_booths; i++){
		printf("[%c]", buffer[i]);
	}
	printf("  <-| Voting!\n");
	
}

void print_voting_status(char party, int threadid, char status ){
	int i;
	char *voting_status= NULL;
	char *party_status = NULL;

	if( party == 'R' ){
		party_status = "Republican";
	}else if( party == 'D' ){
		party_status = "Democrat";
	}else{
		party_status = "Independent";
	}
	
	printf("%s    %d \t|->  ", party_status, threadid);
	for(i = 0; i < num_booths; i++){
		printf("[%c]", buffer[i]);
	}
	
	if( status == 'E' ){
		voting_status = "Entering the polling station";
	}else if( status == 'W' ){
		voting_status = "Waiting on a voting booth";
	}else{
		voting_status = "Leaving the polling station";
	}
	
	printf("  <-| %s\n", voting_status);
	total_entered++;
        printf("total enterd = %d\n", total_entered);
	
}

void *republican(void *threadid){
	int tid = (intptr_t)threadid;
	char party = 'R';
	int i, booth_num;

	//print waiting for polling station to open
        semaphore_wait(&printing_mutex);
	print_wait_polling_station(party, tid);
	semaphore_post(&printing_mutex);
        //print waiting for polling station to open
            /* semaphore_wait(&printing_mutex);
        if(total_waiting <= (total_voting-1)){
            print_wait_polling_station(party, tid);
         }
        semaphore_post(&printing_mutex);
        */	


	//waiting until all threads can enter polling station after two seconds
	semaphore_wait(&barrier);
	semaphore_post(&barrier);
	
	//print that you are entering the polling station
	semaphore_wait(&printing_mutex);
	print_voting_status(party, tid, 'E');
	semaphore_post(&printing_mutex);
	
	/*
	 * you entered the building now "sign-in and register" by waiting 
	 * a random number between 0 and 50,000 microseconds
	 */
	 usleep(random()%50000);
	 printf("sleeping\n");
	 /*
	  * Want everyone to say they entered the building before we can 
	  * start changing voting statuses. If you "registered" quickly 
	  * you may get to jump in line first!
	  */
	 if( total_entered == total_voting ){
             printf("inside if total entered == total voting\n");
		 semaphore_post(&can_vote);
	 }
	 
	 //wait until all have entered the building
	 semaphore_wait(&can_vote);
	 semaphore_post(&can_vote);
	 printf("can vote republicans\n");
	 /*
	  * Let's do some voting!
	  */
	//while(true){
		
	semaphore_wait(&mutex_lineup);
        printf("%c made it through the mutex_lineup!\n", party);
	if( dem_inline != 0){ //there are democrats in line to vote
		semaphore_wait(&rep_barrier);
		sleep(1); //walking in line
		rep_inline++;
		semaphore_post(&mutex_lineup);//let other people try and line up 
		semaphore_wait(&dem_barrier);
		
		/*
		 * vote
		 */
		print_voting_status(party, tid, 'W'); //print waiting on a voting booth
		semaphore_wait(&voting_booths); //see if booths are available
		
		//if we hit here we have a voting booth so vote
		for(i = 0; i < num_booths; i++){
			if( buffer[i] == '.' ){
				buffer[i] = 'R';
				booth_num = i; 
				break; //we're in our first open booth so jump out
			}
		}
		//print that we are now casting our vote a.k.a sleeping!
		semaphore_wait(&printing_mutex);
		print_voting(party ,tid, booth_num);
		usleep(random()%100000);
		semaphore_post(&printing_mutex);
		
		//finished voting
		print_voting_status(party, tid, 'L'); //print leaving a voting booth
		semaphore_post(&dem_barrier); //let a democrat know you left
		semaphore_post(&voting_booths); //release the booth you voted in
		rep_inline--;
	}
	else{//no democrats in line
		sleep(1); //walking in line
		rep_inline++;//we're in line waiting to vote
		semaphore_post(&mutex_lineup); //let other people try and line up
		semaphore_wait(&dem_barrier);
		
		/*
		 * vote
		 */
		print_voting_status(party, tid, 'W'); //print waiting on a voting booth
		semaphore_wait(&voting_booths); //see if booths are available
		
		//if we hit here we have a voting booth so vote
		for(i = 0; i < num_booths; i++){
			if( buffer[i] == '.' ){
				buffer[i] = 'R';
				booth_num = i; 
				break; //we're in our first open booth so jump out
			}
		}
		//print that we are now casting our vote a.k.a sleeping!
		semaphore_wait(&printing_mutex);
		print_voting(party ,tid, booth_num);
		usleep(random()%100000);
		semaphore_post(&printing_mutex);
		
		//finished voting
		print_voting_status(party, tid, 'L'); //print leaving a voting booth
		semaphore_post(&dem_barrier); //let a democrat know you left
		semaphore_post(&voting_booths); //release the booth you voted in
		rep_inline--;
		
	}		

		//exit thread.
		pthread_exit(NULL);		
		/* 
		 * It's done voting so we don't need the thread anymore. 
		 * For the above reason I don't think we need waiting_rep, waiting_ind
		 * and waiting_dem....
		 * Also, why I don't think we need a while loop either. when it reaches
		 * the end we kill it. It should only go through once to vote not as many
		 * times as it wants to. 
		*/
}

void *democrat(void *threadid){
	int tid = (intptr_t)threadid;
	char party = 'D';
        int i, booth_num;
	/*
	 * need a check because we don't want this to print again after
	 * all threads have gotten to this point
	 */
	semaphore_wait(&printing_mutex);
	print_wait_polling_station(party, tid);
	semaphore_post(&printing_mutex);
        
        //print waiting for polling station to open
        /*semaphore_wait(&printing_mutex);
        if(total_waiting <= (total_voting-1)){
            print_wait_polling_station(party, tid);
        }
        semaphore_post(&printing_mutex);
	*/

	//waiting until all threads can enter polling station after two seconds
	semaphore_wait(&barrier);
	semaphore_post(&barrier);
	
	//print that you are entering the polling station
	semaphore_wait(&printing_mutex);
	print_voting_status(party, tid, 'E');
	semaphore_post(&printing_mutex);
	
	/*
	 * you entered the building now "sign-in and register" by waiting 
	 * a random number between 0 and 50,000 microseconds
	 */
	 usleep(random()%50000);
	 printf("sleeping\n");
	 /*
	  * Want everyone to say they entered the building before we can 
	  * start changing voting statuses. If you "registered" quickly 
	  * you may get to jump in line!
	  */
	 if( total_entered == total_voting ){
             printf("inside if total entered == total voting\n");
		 semaphore_post(&can_vote);
	 }
	 
	 //wait until all have entered the building
	 semaphore_wait(&can_vote);
	 semaphore_post(&can_vote);
         printf("can vote democrats\n");
	 
	/*
	  * Let's do some voting!
	  */
	//while(true){
		
	semaphore_wait(&mutex_lineup);
        printf("%c made it through the mutex_lineup!\n", party);

	if( rep_inline != 0){ //there are democrats in line to vote
		semaphore_wait(&dem_barrier);
		sleep(1); //walking in line
		dem_inline++;
		semaphore_post(&mutex_lineup);//let other people try and line up 
		semaphore_wait(&rep_barrier);
		
		/*
		 * vote
		 */
		print_voting_status(party, tid, 'W'); //print waiting on a voting booth
		semaphore_wait(&voting_booths); //see if booths are available
		
		//if we hit here we have a voting booth so vote
		for(i = 0; i < num_booths; i++){
			if( buffer[i] == '.' ){
				buffer[i] = 'D';
				booth_num = i; 
				break; //we're in our first open booth so jump out
			}
		}
		//print that we are now casting our vote a.k.a sleeping!
		semaphore_wait(&printing_mutex);
		print_voting(party ,tid, booth_num);
		usleep(random()%100000);
		semaphore_post(&printing_mutex);
		
		//finished voting
		print_voting_status(party, tid, 'L'); //print leaving a voting booth
		semaphore_post(&rep_barrier); //let a democrat know you left
		semaphore_post(&voting_booths); //release the booth you voted in
		rep_inline--;
	}
	else{//no democrats in line
		sleep(1); //walking in line
		dem_inline++;//we're in line waiting to vote
		semaphore_post(&mutex_lineup); //let other people try and line up
		semaphore_wait(&rep_barrier);
		
		/*
		 * vote
		 */
		print_voting_status(party, tid, 'W'); //print waiting on a voting booth
		semaphore_wait(&voting_booths); //see if booths are available
		
		//if we hit here we have a voting booth so vote
		for(i = 0; i < num_booths; i++){
			if( buffer[i] == '.' ){
				buffer[i] = 'D';
				booth_num = i; 
				break; //we're in our first open booth so jump out
			}
		}
		//print that we are now casting our vote a.k.a sleeping!
		semaphore_wait(&printing_mutex);
		print_voting(party ,tid, booth_num);
		usleep(random()%100000);
		semaphore_post(&printing_mutex);
		
		//finished voting
		print_voting_status(party, tid, 'L'); //print leaving a voting booth
		semaphore_post(&rep_barrier); //let a democrat know you left
		semaphore_post(&voting_booths); //release the booth you voted in
		dem_inline--;
		
	}		

		//exit thread.
		pthread_exit(NULL);		
		/* 
		 * It's done voting so we don't need the thread anymore. 
		 * For the above reason I don't think we need waiting_rep, waiting_ind
		 * and waiting_dem....
		 * Also, why I don't think we need a while loop either. when it reaches
		 * the end we kill it. It should only go through once to vote not as many
		 * times as it wants to. 
		*/
}

void *independent(void *threadid){
	int tid = (intptr_t)threadid;
	char party = 'I';
        int i, booth_num;
	/*
	 * need a check because we don't want this to print again after
	 * all threads have gotten to this point
	 */
        semaphore_wait(&printing_mutex);
	print_wait_polling_station(party, tid);
	semaphore_post(&printing_mutex);
                
        //print waiting for polling station to open
        /*semaphore_wait(&printing_mutex);
        if(total_waiting <= (total_voting-1)){
            print_wait_polling_station(party, tid);
        }
        semaphore_post(&printing_mutex);
	*/
	//waiting until all threads can enter polling station after two seconds
	semaphore_wait(&barrier);
	semaphore_post(&barrier);
	
	//print that you are entering the polling station
	semaphore_wait(&printing_mutex);
	print_voting_status(party, tid, 'E');
	semaphore_post(&printing_mutex);
	
	/*
	 * you entered the building now "sign-in and register" by waiting 
	 * a random number between 0 and 50,000 microseconds
	 */
	 usleep(random()%50000);
	 printf("sleeping\n");
	 /*
	  * Want everyone to say they entered the building before we can 
	  * start changing voting statuses. If you "registered" quickly 
	  * you may get to jump in line!
	  */
	 if( total_entered == total_voting ){
             printf("inside if total entered == total voting\n");
		 semaphore_post(&can_vote);
	 }
	 
	 //wait until all have entered the building
	 semaphore_wait(&can_vote);
	 semaphore_post(&can_vote);
	 printf("can vote independents\n");
	/*
	* Let's do some voting!
	*/
	
	semaphore_wait(&mutex_lineup);
        printf("%c made it through the mutex_lineup!\n", party);

	semaphore_post(&mutex_lineup);//let other people try and line up 
	sleep(1); //walking in line
	
	/*
	 * vote
	 */
	print_voting_status(party, tid, 'W'); //print waiting on a voting booth
	semaphore_wait(&voting_booths); //see if booths are available
	
	//if we hit here we have a voting booth so vote
	for(i = 0; i < num_booths; i++){
		if( buffer[i] == '.' ){
			buffer[i] = 'I';
			booth_num = i; 
			break; //we're in our first open booth so jump out
		}
	}
	//print that we are now casting our vote a.k.a sleeping!
	semaphore_wait(&printing_mutex);
	print_voting(party ,tid, booth_num);
	usleep(random()%100000);
	semaphore_post(&printing_mutex);
	
	//finished voting
	print_voting_status(party, tid, 'L'); //print leaving a voting booth
	semaphore_post(&voting_booths); //release the booth you voted in

	//exit thread.
	pthread_exit(NULL);		
	/* 
	 * It's done voting so we don't need the thread anymore. 
	 * For the above reason I don't think we need waiting_rep, waiting_ind
	 * and waiting_dem....
	 * Also, why I don't think we need a while loop either. when it reaches
	 * the end we kill it. It should only go through once to vote not as many
	 * times as it wants to. 
         */
}
