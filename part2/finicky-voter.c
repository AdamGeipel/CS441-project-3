/*
 *
 *
 * CS 441/541: Finicky Voter (Assignment 2 Part 2)
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

    total_voting = num_republicans + num_democrats + num_independents;
    printf("Number of Voting Booths : %d\n", num_booths);
    printf("Number of Republican : %d\n", num_republicans);
    printf("Number of Democrat : %d\n", num_democrats);
    printf("Number of Independent : %d\n", num_independents);
    printf("------------------------+---------------------------------------------------------------------------------------\n");

    
    //create space for buffer (booths)
    buffer = malloc(sizeof(int) * num_booths);
 
    //buffer is initially empty with '.'
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
    
    srand(time(NULL)); 
    
    create_and_join_threads();
    
    //free everything
    free(buffer);
    buffer = NULL;
    free(rep_threads);
    rep_threads = NULL;
    free(dem_threads);
    dem_threads = NULL;
    free(ind_threads);
    ind_threads = NULL;


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
    //now open up polling station
    semaphore_post(&barrier);
   
    
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
    
    //waiting to enter polling station
    total_waiting++; 
    
    if(total_waiting == total_voting){
        printf("------------------------+---------------------------------------------------------------------------------------\n");
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
    //keep track to we know when all threads have "entered" the building
    total_entered++;
   
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


void* republicans(void *threadid){
    int tid = (intptr_t)threadid;
    char party = 'R';
    int i, booth_num;
    
    //say we're waiting for the polling station to open
    print_wait_polling_station(party, tid);
    
    //enter into the polling station one by one
    semaphore_wait(&barrier);
    semaphore_post(&barrier);
   
    //say we are entering the building
    print_voting_status(party,tid, 'E');
    
    //if everyone entered the building let them know they can vote after registering
    if(total_entered == total_voting){
        printf("total entered: %d", total_entered);
        semaphore_post(&can_vote);
    }

    //you've entered so simulate registering by sleeping
    usleep(random()%500000);
    
    //wait until everyone enters the polling station before we start voting
    semaphore_wait(&can_vote);
    semaphore_post(&can_vote);
    
    /*
     * voting
     */
    //walk past register station
    usleep(random()%100000);
       
    //every thread will hit this so it needs a post in the if and the else as a thread will only 
    //enter one of those based on whether or not the opposing party is in line
    semaphore_wait(&mutex_lineup);
    
    //if no democrats are waiting in line, goahead
    if(dem_inline == 0){

        //increment number of republicans that are in line
        rep_inline++;
        //print we are waiting on a booth
        print_voting_status(party, tid, 'W');
        //increment the number of people in line/voting
        semaphore_wait(&mutex_in);
        num_inline++;
        semaphore_post(&mutex_in);
        //let someone else enter the line
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
                buffer[i] = 'R';
                booth_num = i;
                break;
            }   
        }
            
        //say that you are voting!
        print_voting(party,tid, booth_num);
        usleep(random()%1000000);
            
        //you finished voting so leave the building and clear you booth
        print_voting_status(party, tid, 'L');
        buffer[booth_num] = '.';
        semaphore_post(&free_booths);
            
        
        semaphore_wait(&mutex_in);
        num_inline--;
        //there is a spot in line
        if( num_inline < num_booths ){
            //release the number of independents per open spot
            for(i = 0; i < (num_booths - num_inline); i++){
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

           
    }else{
            
        //protected by mutex_lineup
        rep_inline++;
        //wait if there are democrats in line/voting
        semaphore_wait(&reps_wait_on_dems);
           
        //print we are waiting on a booth
        print_voting_status(party, tid, 'W');
        
        //increment the number of people in line/voting
        semaphore_wait(&mutex_in);
        num_inline++;
        semaphore_post(&mutex_in);
        
        //let someone else enter the line
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
                buffer[i] = 'R';
                booth_num = i;
                break;
            }   
        }
            
        //say that you are voting!
        print_voting(party, tid, booth_num);
        usleep(random()%1000000);
            
        //done voting so say we're leaving and clear your booth 
        print_voting_status(party, tid, 'L');
        buffer[booth_num] = '.';
            
        semaphore_post(&free_booths);
            
        semaphore_wait(&mutex_in);
        num_inline--;
        //there is a spot in line
        if( num_inline < num_booths ){
            //release the number of independents per open spot
            for(i = 0; i < (num_booths - num_inline); i++){
                semaphore_post(&inds_waiting_for_line);
            }
        }
        semaphore_post(&mutex_in);
            
        semaphore_wait(&protect_count);
        rep_inline--;
        //if no republicans in line
        if(rep_inline == 0){
            for(i=0; i < dem_inline; i++){
                //when there are no more republicans in line, release all of the democrats 
                //that were backed up waiting
                semaphore_post(&dem_wait_on_reps);
            }
        }
        semaphore_post(&protect_count);

           
    }
        
    pthread_exit(NULL);    
}

void* democrats(void *threadid){
    int tid = (intptr_t)threadid;
    char party = 'D';
    int i, booth_num;
   
    //say we're waiting on polling station to open
    print_wait_polling_station(party, tid);
    
    //enter into the polling station one by one
    semaphore_wait(&barrier);
    semaphore_post(&barrier);
    
    //say we're entering the building
    print_voting_status(party, tid, 'E');

    //if everyone has entered then let them know they can vote
    if(total_entered == total_voting){
        printf("total entered: %d", total_entered);
        semaphore_post(&can_vote);
    }
    
    //you've entered to simulate registering by sleeping
    usleep(random()%500000);
    
    //wait until everyone has entered to vote
    semaphore_wait(&can_vote);
    semaphore_post(&can_vote);
   
    /*
     * voting
     */
    
    //walk past register station
    usleep(random()%100000);
        
    //every thread will hit this so it needs a post in the if and the else as a thread will only 
    //enter one of those based on whether or not the opposing party is in line
    semaphore_wait(&mutex_lineup);
    if(rep_inline == 0){
            
        dem_inline++;
        //waiting on a booth
        print_voting_status(party, tid, 'W');
        //increment number of people in line/voting
        semaphore_wait(&mutex_in);
        num_inline++;
        semaphore_post(&mutex_in);
            
        //let someone else in line
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
            
        //say we are voting
        print_voting(party, tid, booth_num);
        usleep(random()%1000000);
            
        //we're done voting so leave and clear your booth
        print_voting_status(party, tid, 'L');
        buffer[booth_num] = '.';
        semaphore_post(&free_booths);
            
        semaphore_wait(&mutex_in);
        num_inline--;
        //there is a spot in line
        if( num_inline < num_booths ){
            //release the number of independents per open spot
            for(i = 0; i < (num_booths - num_inline); i++){
                semaphore_post(&inds_waiting_for_line);
            }
        }
        semaphore_post(&mutex_in);

            
        semaphore_wait(&protect_count);
        dem_inline--;
        //if no more democrats in line
        if(dem_inline == 0){
            for(i=0; i < rep_inline; i++){
                //when there are no more democrats in line, release all of the republicans 
                //that were backed up waiting
                semaphore_post(&reps_wait_on_dems);
            }
        }
        semaphore_post(&protect_count);

           
    }else{
            
        //protected by mutex_lineup
        dem_inline++;
        //wait while there are republicans in line
        semaphore_wait(&dem_wait_on_reps);
            
        //say we are waiting for a booth
        print_voting_status(party, tid, 'W');
        //increment number of people in line/voting
        semaphore_wait(&mutex_in);
        num_inline++;
        semaphore_post(&mutex_in);
        
        //let someone else in line
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
            
        //say we are voting
        print_voting(party, tid, booth_num);
        usleep(random()%1000000);
            
        //done voting so leave and clear your booth
        print_voting_status(party, tid, 'L');
        buffer[booth_num] = '.';
        semaphore_post(&free_booths);
            
        semaphore_wait(&mutex_in);
        num_inline--;
        //there is a spot in line
        if( num_inline < num_booths ){
            //release the number of independents per open spot
            for(i = 0; i < (num_booths - num_inline); i++){
                semaphore_post(&inds_waiting_for_line);
            }
        }
        semaphore_post(&mutex_in);


        semaphore_wait(&protect_count);
        dem_inline--;
        //if no more democrats in line
        if(dem_inline == 0){
            for(i=0; i < rep_inline; i++){
                //when there are no more democrats in line, release all of the republicans 
                //that were backed up waiting
                semaphore_post(&reps_wait_on_dems);
            }
        }
        semaphore_post(&protect_count);

          
    }
        
   
    pthread_exit(NULL);
}

void* independents(void *threadid){
    int tid = (intptr_t)threadid;
    char party = 'I';
    int i, booth_num;

    //say we're waiting on the polling station
    print_wait_polling_station(party, tid);
    
    //enter the bulding one by one
    semaphore_wait(&barrier);
    semaphore_post(&barrier);
   
    
    //say we entered the building
    print_voting_status(party, tid, 'E');
   
    
    //after everyone enters then tell them they can vote after registering
    if(total_entered == total_voting){
        semaphore_post(&can_vote);
    }

    //you've entered to simulate registering by sleeping
    usleep(random()%500000);
    
    //wait until everyone has entered the building to start voting
    semaphore_wait(&can_vote);
    semaphore_post(&can_vote);
    
    
    
        
    //walk past register station
    usleep(random()%100000);
        
    /*
     *if there aren't as many people in line as their are number of booths, goahead
     *________________________
     *'num_inline' is incremented everytime a rep or dem enters a line and is
     *decremented everytime a rep or dem leaves the voting booths
     */
    if(num_inline != num_booths){
        //increment there is another person in line/voting
        semaphore_wait(&mutex_in);
        num_inline++;
        semaphore_post(&mutex_in);
        
        //say we're waiting on a booth
        print_voting_status(party, tid, 'W');

        //simualte walking in line
        sleep(1);
            
        /*
         * check if there are any open booths
         * multiple threads could be entering here if multiple booths
         * are open hence why we need the printing_mutex inside the print methods
         */
        semaphore_wait(&free_booths);
        for(i = 0; i < num_booths; i++){
            if( buffer[i] == '.'){
                buffer[i] = 'I';
                booth_num = i;
                break;
            }
                
        }
            
        //say we're voting
        print_voting(party, tid, booth_num);
        usleep(random()%1000000);
              
        //done voting so leave and clear youre booth
        print_voting_status(party, tid, 'L');
        buffer[booth_num] = '.';
        
        semaphore_wait(&mutex_in);
        num_inline--;
        //there is a spot in line
        if( num_inline < num_booths ){
            //release the number of independents per open spot
            for(i = 0; i < (num_booths - num_inline); i++){
                semaphore_post(&inds_waiting_for_line);
            }
        }
        semaphore_post(&mutex_in);

        semaphore_post(&free_booths);
            
          
    }else{
        //otherwise sit and wait until there are less people in line
        semaphore_wait(&mutex_lineup);
        
        semaphore_wait(&inds_waiting_for_line);
        //increment there is another person in line/voting
        semaphore_wait(&mutex_in);
        num_inline++;
        semaphore_post(&mutex_in);
        
        //say wer're waiting on a booth
        print_voting_status(party, tid, 'W');
        semaphore_post(&mutex_lineup);

        //simualte walking in line
        sleep(1);
        
        
        semaphore_wait(&free_booths);
        for(i = 0; i < num_booths; i++){
            if( buffer[i] == '.'){
                buffer[i] = 'I';
                booth_num = i;
                break;
            }
                
        }
            
        //say we're voting!
        print_voting(party, tid, booth_num);
        //simulate voting
        usleep(random()%1000000);
              
        //done voting so leave and clear your booth
        print_voting_status(party, tid, 'L');
        buffer[booth_num] = '.';
        
        
        semaphore_wait(&mutex_in);
        num_inline--;
        //there is a spot in line
        if( num_inline < num_booths ){
            //release the number of independents per open spot
            for(i = 0; i < (num_booths - num_inline); i++){
                semaphore_post(&inds_waiting_for_line);
            }
        }
            
        semaphore_post(&mutex_in);

        semaphore_post(&free_booths);
            
    }
  
    pthread_exit(NULL);
}
