
/*
 *
 *
 * CS 441/541: Finicky Voter (Assignment 2 Part 2)
 *
 */
#include "finicky-voter.h"

int main(int argc, char * argv[]) {
    srand(time(NULL));
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

    total_voting = num_republicans + num_democrats + num_independents;
    printf("Number of Voting Booths : %d\n", num_booths);
    printf("Number of Republican : %d\n", num_republicans);
    printf("Number of Democrat : %d\n", num_democrats);
    printf("Number of Independent : %d\n", num_independents);
    printf("----------------------+---------------------------------------------------------------------------------------\n");

    
    //making room for the buffer and threads in memory
    buffer = malloc(sizeof(int) * num_booths);
 
    //initialize the array to empty
    for ( i = 0; i < num_booths; i++){
        buffer[i] = '.';
    }
    
    //create our semaphores
    if ((semaphore_create(&can_vote, 0)) == -1)
        {
            fprintf(stderr, "Error: semaphore cannot be created.\n");
            exit(-1);
        }
    if ((semaphore_create(&reps_wait_on_dems, 0)) == -1){
        fprintf(stderr, "Error: semaphore cannot be created.\n");
        exit(-1);
    }
    if ((semaphore_create(&dem_wait_on_reps, 0)) == -1){
        fprintf(stderr, "Error: semaphore cannot be created.\n");
        exit(-1);
    }
    if ((semaphore_create(&free_booths, num_booths)) == -1){
        fprintf(stderr, "Error: semaphore cannot be created.\n");
        exit(-1);
    }
    if ((semaphore_create(&protect_count, 1)) == -1){
        fprintf(stderr, "Error: semaphore cannot be created.\n");
        exit(-1);
    }
    if ((semaphore_create(&mutex_lineup, 1)) == -1){
        fprintf(stderr, "Error: semaphore cannot be created.\n");
        exit(-1);
    }
    if ((semaphore_create(&barrier, 0)) == -1){
        fprintf(stderr, "Error: semaphore cannot be created.\n");
        exit(-1);
    }
    if ((semaphore_create(&printing_mutex, 1)) == -1){
        fprintf(stderr, "Error: semaphore cannot be created.\n");
        exit(-1);
    }
    if ((semaphore_create(&mutex_in, 1)) ==-1){
        fprintf(stderr, "Error: semaphore cannot be created.\n");
        exit(-1);
    }
    if ((semaphore_create(&inds_waiting_for_line, 0)) ==-1){
        fprintf(stderr, "Error: semaphore cannot be created.\n");
        exit(-1);
    }
  
    
    create_and_join_threads();

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
        rc = pthread_create(&(rep_threads[i]), NULL, republicans, (void*)(intptr_t)i);
        if (rc != 0){
            fprintf(stderr, "Error: failed to create thread \n");
            exit(-1);
        }
    }
    
    dem_threads = (pthread_t *)malloc(sizeof(pthread_t) * num_democrats);
    //create democrat threads
    for(i = 0; i < num_democrats; i++){
        rc = pthread_create(&(dem_threads[i]), NULL, democrats, (void*)(intptr_t)i);
        if (rc != 0){
            fprintf(stderr, "Error: failed to create thread \n");
            exit(-1);
        }
    }
    
    ind_threads = (pthread_t *)malloc(sizeof(pthread_t) * num_independents);
    //create independent threads
    for(i = 0; i < num_independents; i++){
        rc = pthread_create(&(ind_threads[i]), NULL, independents, (void*)(intptr_t)i);
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
    //put printing mutex inside method so you don't have to type it before
    //and after everytime you want to call a print method
    semaphore_wait(&printing_mutex);
    char *party_status = NULL;
    
    if( party == 'R' ){
        party_status = "Republican ";
    }else if( party == 'D' ){
        party_status = "Democrat   ";
    }else{
        party_status = "Independent";
    }
    
    printf("%s %2d\t\t|->  ", party_status, threadid);
    for(i = 0; i < num_booths; i++){
        printf("[%c]", buffer[i]);
    }
    printf("  <-| Waiting for polling station to open...\n");
    
    //total_waiting++; //waiting to enter polling station
    
    if(total_waiting == total_voting){
        printf("----------------------+---------------------------------------------------------------------------------------\n");
    }
    semaphore_post(&printing_mutex);
}

void print_voting_status(char party, int threadid, char status ){
    int i;
    //put printing mutex inside method so you don't have to type it before
    //and after everytime you want to call a print method
    semaphore_wait(&printing_mutex);
    char *voting_status= NULL;
    char *party_status = NULL;

    if( party == 'R' ){
        party_status = "Republican ";
    }else if( party == 'D' ){
        party_status = "Democrat   ";
    }else{
        party_status = "Independent";
    }
    
    printf("%s %2d\t\t|->  ", party_status, threadid);
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
    //total_entered++;
    //  printf("total enterd = %d\n", total_entered);
    semaphore_post(&printing_mutex);
}

void print_voting( char party, int threadid, int booth ){
    int i;
    //put printing mutex inside method so you don't have to type it before
    //and after everytime you want to call a print method
    semaphore_wait(&printing_mutex);
    char *party_status = NULL;
    
    if( party == 'R' ){
        party_status = "Republican ";
    }else if( party == 'D' ){
        party_status = "Democrat   ";
    }else{
        party_status = "Independent";
    }
    
    printf("%s %2d   in %3d |->  ", party_status, threadid, booth);
    
    for(i = 0; i < num_booths; i++){
        printf("[%c]", buffer[i]);
    }
    printf("  <-| Voting!\n");
    semaphore_post(&printing_mutex);
}

<<<<<<< HEAD
void *republican(void *threadid){
	int tid = (intptr_t)threadid;
	char party = 'R';
	int i, booth_num;

	//print waiting for polling station to open
        semaphore_wait(&printing_mutex);
		print_wait_polling_station(party, tid);
	semaphore_post(&printing_mutex);

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
	
	 /*
	  * Want everyone to say they entered the building before we can 
	  * start changing voting statuses. If you "registered" quickly 
	  * you may get to jump in line first!
	  */
	 if( total_entered == total_voting ){
           
		 semaphore_post(&can_vote);
	 }
	 
	 //wait until all have entered the building
	 semaphore_wait(&can_vote);
	 semaphore_post(&can_vote);
	
	 /*
	  * Let's do some voting!
	  */
	while(1){
		
	semaphore_wait(&mutex_lineup);
      
	if( dem_inline != 0){ //there are democrats in line to vote
		//semaphore_wait(&rep_barrier);
		
		while(dem_inline !=0) {} //busy wait
		
		sleep(1); //walking in line
		rep_inline++;
		printf("republicans in line: %d\n", rep_inline);
		

		semaphore_post(&mutex_lineup);//let other people try and line up 
		//semaphore_wait(&dem_barrier);
		
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
		semaphore_wait(&printing_mutex);
			print_voting_status(party, tid, 'L'); //print leaving a voting booth
		semaphore_post(&printing_mutex);

		buffer[booth_num] = '.';
		//semaphore_post(&dem_barrier); //let a democrat know you left
		semaphore_post(&voting_booths); //release the booth you voted in
		

		semaphore_wait(&rep_in_line_mutex);
			rep_inline--;
			printf("republicans in line: %d\n", rep_inline);
		semaphore_post(&rep_in_line_mutex);
		break;
	}
	else{//no democrats in line
		sleep(1); //walking in line
		rep_inline++;//we're in line waiting to vote
		printf("republicans in line: %d\n", rep_inline);
		

		

		semaphore_post(&mutex_lineup); //let other people try and line up
		//semaphore_wait(&dem_barrier);
		
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
		buffer[booth_num] = '.';
		semaphore_wait(&printing_mutex);
			print_voting_status(party, tid, 'L'); //print leaving a voting booth
		semaphore_post(&printing_mutex);
		//semaphore_post(&dem_barrier); //let a democrat know you left
		semaphore_post(&voting_booths); //release the booth you voted in
		
		

		semaphore_wait(&rep_in_line_mutex);
			rep_inline--;
			printf("republicans in line: %d\n", rep_inline);
		semaphore_post(&rep_in_line_mutex);
		break;
		}
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
=======

void* republicans(void *threadid){
    int tid = (intptr_t)threadid;
    char party = 'R';
    int i, booth_num;
    
    //protect counters and prints the entering of the polling station
    semaphore_wait(&protect_count);
    if(total_waiting <= (total_voting - 1 )){
        print_wait_polling_station(party, tid);
    }        
    total_waiting++;
    semaphore_post(&protect_count);
    
    //make sure all the threads are created and have arrived to the polling station
    semaphore_wait(&barrier);
    semaphore_post(&barrier);
   
    //enter the building
    semaphore_wait(&protect_count);
    print_voting_status(party,tid, 'E');
    total_entered++;
    semaphore_post(&protect_count);
    
    //our flag to signal that everyone entered
    if(total_entered == total_voting){
        semaphore_post(&can_vote);
    }

    //you've entered to simulate registering by sleeping
    usleep(random()%500000);
    
    //make sure everyone enters the polling station before we start voting
    semaphore_wait(&can_vote);
    semaphore_post(&can_vote);
    //usleep(random()%100000);
    
    /*
     * voting
     */
    while(1){
        //walk past register station
        usleep(random()%100000);
       
        //every thread will hit this so it needs a post in the if and the else as a thread will only 
        //enter one of those based on whether or not the opposing party is in line
        semaphore_wait(&mutex_lineup);
        //check to see if democrats are waiting
        if(dem_inline == 0){
            
            //protecting counters/printing enter at polling station
            
            rep_inline++;
            print_voting_status(party, tid, 'W');
            semaphore_wait(&mutex_in);
            in++;
            semaphore_post(&mutex_in);
            
            semaphore_post(&mutex_lineup);
            
            //simulate walking in line
            sleep(1);

            /*
             * check if there are any open booths
             * multiple threads could be entering here if multiple booths
             * are open hence why we need the printing_mutex inside the print methods
             */
            semaphore_wait(&free_booths);
            //replaces the empty booth with a republican 
            for(i = 0; i < num_booths; i++){
                if( buffer[i] == '.'){
                    buffer[i] = 'R';
                    booth_num = i;
                    break;
                }   
            }
            
            print_voting(party,tid, booth_num);
            usleep(random()%1000000);
               
            print_voting_status(party, tid, 'L');
            
            //reset the buffer
            buffer[booth_num] = '.';
            semaphore_post(&free_booths);
            
            semaphore_wait(&mutex_in);
            in--;
            //there is a spot in line
            if( in < num_booths ){
                //release the number of independents per open spot
                for(i = 0; i < (num_booths - in); i++){
                    semaphore_post(&inds_waiting_for_line);
                }
            }
            semaphore_post(&mutex_in);

            
            semaphore_wait(&protect_count);
            //republican leaves booth
            rep_inline--;
            if(rep_inline == 0){
                for(i=0; i < dem_inline; i++){
                    //when there are no more republicans in line, release all of the democrats 
                    //that were backed up waiting
                    semaphore_post(&dem_wait_on_reps);
                }
            }
            semaphore_post(&protect_count);

            break;
        }else{
            
            //protected by mutex_lineup;
            rep_inline++;
            //wait if there are democrats in the booth
            semaphore_wait(&reps_wait_on_dems);
            
            //this code is the same as the true condition in this statement
            print_voting_status(party, tid, 'W');
            semaphore_wait(&mutex_in);
            in++;
            semaphore_post(&mutex_in);

            semaphore_post(&mutex_lineup);
            
            //simulate walking into line
            sleep(1);
            
            /*
             * check if there are any open booths
             * multiple threads could be entering here if multiple booths
             * are open hence why we need the printing_mutex inside the print methods
             */
            semaphore_wait(&free_booths);

            for(i = 0; i < num_booths; i++){
                if( buffer[i] == '.'){
                    buffer[i] = 'R';
                    booth_num = i;
                    break;
                }   
            }
            
            print_voting(party, tid, booth_num);
            usleep(random()%1000000);
            
            print_voting_status(party, tid, 'L');
            buffer[booth_num] = '.';
            
            semaphore_post(&free_booths);
            
            semaphore_wait(&mutex_in);
            in--;
            //there is a spot in line
            if( in < num_booths ){
                //release the number of independents per open spot
                for(i = 0; i < (num_booths - in); i++){
                    semaphore_post(&inds_waiting_for_line);
                }
            }
            semaphore_post(&mutex_in);
            
            semaphore_wait(&protect_count);
            rep_inline--;
            if(rep_inline == 0){
                for(i=0; i < dem_inline; i++){
                    //when there are no more republicans in line, release all of the democrats 
                    //that were backed up waiting
                    semaphore_post(&dem_wait_on_reps);
                }
            }
            semaphore_post(&protect_count);

            break;
        }
        //usleep(random()%100000);
    }
    //exit the thread
    pthread_exit(NULL);    
>>>>>>> 9138e0ad296ba1bc4e6c240e93c01802f40b1c1d
}

void* democrats(void *threadid){
    int tid = (intptr_t)threadid;
    char party = 'D';
    int i, booth_num;
    
    //check to see if we made all of the threads
    semaphore_wait(&protect_count);
    if(total_waiting <= (total_voting - 1 )){
        print_wait_polling_station(party, tid);
    }        
    total_waiting++;
    semaphore_post(&protect_count);
    
    //make sure all the threads are created and have arrived to the polling station
    semaphore_wait(&barrier);
    semaphore_post(&barrier);
    
    //entering the building
    semaphore_wait(&protect_count);
    print_voting_status(party, tid, 'E');
    total_entered++; 
    semaphore_post(&protect_count);

    //check to see if all the voters have entered
    if(total_entered == total_voting){
        semaphore_post(&can_vote);
    }
    
    //you've entered to simulate registering by sleeping
    usleep(random()%500000);
    
    //make sure everyone enters the polling station before we start voting
    semaphore_wait(&can_vote);
    semaphore_post(&can_vote);
   
    /*
     * voting
     */
    
    while(1){
        //walk past register station
        usleep(random()%100000);
        
        //every thread will hit this so it needs a post in the if and the else as a thread will only 
        //enter one of those based on whether or not the opposing party is in line
        semaphore_wait(&mutex_lineup);
        if(rep_inline == 0){
            
            dem_inline++;
            print_voting_status(party, tid, 'W');
            semaphore_wait(&mutex_in);
            in++;
            semaphore_post(&mutex_in);
            
            semaphore_post(&mutex_lineup);
            
            //simulate walking in line
            sleep(1);
            
            /*
             * check if there are any open booths
             * multiple threads could be entering here if multiple booths
             * are open hence why we need the printing_mutex inside the print methods
             */
            semaphore_wait(&free_booths);

            //replace the empty booth with a democrat
            for(i = 0; i < num_booths; i++){
                if( buffer[i] == '.'){
                    buffer[i] = 'D';
                    booth_num = i;
                    break;
                }
                
            }
            
            print_voting(party, tid, booth_num);
            usleep(random()%1000000);
               
            print_voting_status(party, tid, booth_num);
            buffer[booth_num] = '.';
            semaphore_post(&free_booths);
            
            semaphore_wait(&mutex_in);
            in--;
            //there is a spot in line
            if( in < num_booths ){
                //release the number of independents per open spot
                for(i = 0; i < (num_booths - in); i++){
                    semaphore_post(&inds_waiting_for_line);
                }
            }
            semaphore_post(&mutex_in);

            
            semaphore_wait(&protect_count);
            //the democrat leaves the booth
            dem_inline--;
            if(dem_inline == 0){
                for(i=0; i < rep_inline; i++){
                    //when there are no more democrats in line, release all of the republicans 
                    //that were backed up waiting
                    semaphore_post(&reps_wait_on_dems);
                }
            }
            semaphore_post(&protect_count);

            break;
        }else{
            
            //protected by mutex_lineup
            dem_inline++;
            //while(rep_inline !=0){}
            //instead
            semaphore_wait(&dem_wait_on_reps);
            
            //from here on the code is the same as above
            print_voting_status(party, tid, 'W');
            semaphore_wait(&mutex_in);
            in++;
            semaphore_post(&mutex_in);
            semaphore_post(&mutex_lineup);

            //simulate walking in line
            sleep(1);
            
            /*
             * check if there are any open booths
             * multiple threads could be entering here if multiple booths
             * are open hence why we need the printing_mutex inside the print methods
             */
            semaphore_wait(&free_booths);
   
            for(i = 0; i < num_booths; i++){
                if( buffer[i] == '.'){
                    buffer[i] = 'D';
                    booth_num = i;
                    break;
                }
                
            }
            
            print_voting(party, tid, booth_num);
            usleep(random()%1000000);
            
            print_voting_status(party, tid, 'L');
            buffer[booth_num] = '.';
            semaphore_post(&free_booths);
            
            semaphore_wait(&mutex_in);
            in--;
            //there is a spot in line
            if( in < num_booths ){
                //release the number of independents per open spot
                for(i = 0; i < (num_booths - in); i++){
                    semaphore_post(&inds_waiting_for_line);
                }
            }
            semaphore_post(&mutex_in);


            semaphore_wait(&protect_count);
            dem_inline--;
            if(dem_inline == 0){
                for(i=0; i < rep_inline; i++){
                    //when there are no more democrats in line, release all of the republicans 
                    //that were backed up waiting
                    semaphore_post(&reps_wait_on_dems);
                }
            }
            semaphore_post(&protect_count);

            break;
        }
<<<<<<< HEAD
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
	
	 /*
	  * Want everyone to say they entered the building before we can 
	  * start changing voting statuses. If you "registered" quickly 
	  * you may get to jump in line!
	  */
	 if( total_entered == total_voting ){
             
		 semaphore_post(&can_vote);
	 }
	 
	 //wait until all have entered the building
	 semaphore_wait(&can_vote);
	 semaphore_post(&can_vote);
        
	 
	/*
	  * Let's do some voting!
	  */
	while(1){
		
	semaphore_wait(&mutex_lineup);
       

	if( rep_inline != 0){ //there are democrats in line to vote
		//semaphore_wait(&dem_barrier);

		while (rep_inline != 0) {}  //busy wait


		sleep(1); //walking in line
		
		dem_inline++;
		printf("democrats in line: %d\n", dem_inline);


		semaphore_post(&mutex_lineup);//let other people try and line up 
		//semaphore_wait(&rep_barrier);
		
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
		semaphore_wait(&printing_mutex);
			print_voting_status(party, tid, 'L'); //print leaving a voting booth
		semaphore_post(&printing_mutex);

		//semaphore_post(&rep_barrier); //let a democrat know you left
		semaphore_post(&voting_booths); //release the booth you voted in
	

		semaphore_wait(&dem_in_line_mutex);
			dem_inline--;
			printf("democrats in line: %d\n", dem_inline);
		semaphore_post(&dem_in_line_mutex);
		break;
	}
	else{//no democrats in line
		sleep(1); //walking in line
		dem_inline++;//we're in line waiting to vote
		printf("democrats in line: %d\n", dem_inline);

		

		semaphore_post(&mutex_lineup); //let other people try and line up
		//semaphore_wait(&rep_barrier);
		
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
		//semaphore_post(&rep_barrier); //let a democrat know you left
		semaphore_post(&voting_booths); //release the booth you voted in


		semaphore_wait(&dem_in_line_mutex);
			dem_inline--;
			printf("democrats in line: %d\n", dem_inline);
		semaphore_post(&dem_in_line_mutex);
		break;
		}
			
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
=======
        // usleep(random()%100000);
    }
    pthread_exit(NULL);
>>>>>>> 9138e0ad296ba1bc4e6c240e93c01802f40b1c1d
}

void* independents(void *threadid){
    int tid = (intptr_t)threadid;
    char party = 'I';
    int i, booth_num;

    //protect counters and prints the entering of the polling station
    semaphore_wait(&protect_count);
    if(total_waiting <= (total_voting - 1 )){
        print_wait_polling_station(party, tid);
    }        
    total_waiting++;
    semaphore_post(&protect_count);
    
    //make sure all the threads are created and have arrived to the polling station
    semaphore_wait(&barrier);
    semaphore_post(&barrier);
   
    //enter the building
    semaphore_wait(&protect_count);
    print_voting_status(party, tid, 'E');
    total_entered++;
    semaphore_post(&protect_count);
    
    //our flag to signal that everyone entered
    if(total_entered == total_voting){
        semaphore_post(&can_vote);
    }

    //you've entered to simulate registering by sleeping
    usleep(random()%500000);
    
    //make sure everyone enters the polling station before we start voting
    semaphore_wait(&can_vote);
    semaphore_post(&can_vote);
    //usleep(random()%100000);
    
    //vote - independents doesnt have to wait on anybody so the code is significantly less than the other affiliations
    while(1){
        
        //walk past register station
        usleep(random()%100000);
        
        //if there aren't as many people in line as their are number of booths
        //**in is incremented everytime a rep or dem enters a line and is
        //  decremented everytime a rep or dem leaves the voting booths**
        if(in != num_booths){
            semaphore_wait(&mutex_in);
            in++;
            semaphore_post(&mutex_in);
            print_voting_status(party, tid, 'W');

            //simualte walking in line
            sleep(1);
            
            /*
             * check if there are any open booths
             * multiple threads could be entering here if multiple booths
             * are open hence why we need the printing_mutex inside the print methods
             */
            semaphore_wait(&free_booths);
            //put an independent in a voting booth
            for(i = 0; i < num_booths; i++){
                if( buffer[i] == '.'){
                    buffer[i] = 'I';
                    booth_num = i;
                    break;
                }
                
            }
            
            print_voting(party, tid, booth_num);
            usleep(random()%1000000);
               
            print_voting_status(party, tid, 'L');
            buffer[booth_num] = '.';
            semaphore_wait(&mutex_in);
            in--;
            //there is a spot in line
            if( in < num_booths ){
                //release the number of independents per open spot
                for(i = 0; i < (num_booths - in); i++){
                    semaphore_post(&inds_waiting_for_line);
                }
            }
            semaphore_post(&mutex_in);

            semaphore_post(&free_booths);
            
            break;
        }else{
            //otherwise wait until there are less people in line
            semaphore_wait(&mutex_lineup);
            semaphore_wait(&inds_waiting_for_line);
            //while(in == num_booths){}
            semaphore_wait(&mutex_in);
            in++;
            semaphore_post(&mutex_in);
            print_voting_status(party, tid, 'W');
            semaphore_post(&mutex_lineup);

            //simualte walking in line
            sleep(1);
            semaphore_wait(&free_booths);
            
            //put an independent in a voting booth
            for(i = 0; i < num_booths; i++){
                if( buffer[i] == '.'){
                    buffer[i] = 'I';
                    booth_num = i;
                    break;
                }
                
            }
            
            print_voting(party, tid, booth_num);
            //simulate voting
            usleep(random()%1000000);
               
            print_voting_status(party, tid, 'L');
            buffer[booth_num] = '.';
            semaphore_wait(&mutex_in);
            in--;
            //there is a spot in line
            if( in < num_booths ){
                //release the number of independents per open spot
                for(i = 0; i < (num_booths - in); i++){
                    semaphore_post(&inds_waiting_for_line);
                }
            }
            
            semaphore_post(&mutex_in);

            semaphore_post(&free_booths);
            break;

        }
    }
    pthread_exit(NULL);
}
