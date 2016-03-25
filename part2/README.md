# CS441/541 Project 3 - Part 1

## Author(s):

Christa Brehm and Adam Geipel


## Date:

March 24th, 2016


## Description:

The finicky-voter is a program that simulates the operation of a polling booth with a finite number of voting booths and voters from the Republican, Democrat, and Independent parties.

After a two second sleep after all voter threads were generated, voters are let into the polling station one by one. Once inside, the voters wait to register, register, then get in line and wait for a voting booth. Democrats and Republicans are not allowed to be waiting on a voting booth as the same time however, they can be waiting in line and voting with the Independent party.

Voter threads sleep to simulate registering and also when voting to simulate filling out registration paperwork and taking time to thoughtfully vote. They also sleep when they are done registering to simulate walking up to the line and then sleep when they enter the line to simulate walking into the waiting on a booth corral. 


## How to build the software

Run the makefile to compile the file finicky-voter.c

CC = gcc
CFLAGS = -Wall -g -O0 - I../lib
LDFLAGS = -pthreads

Make clean will clean up the folder.


## How to use the software

Finicky-voter has four default arguments:
	
	Number of voting booths: 10
	
	Number of Republicans: 5 
	
	Number of Democrats: 5
	
	Number of Independents: 5

Arguments can be changed from default by specifying all arguments.


## How the software was tested

Software was tested by running the code multiple times with different voting conditions and times spent sleeping.

Debug statements were added before and after each critical section to determine if threads made it through.


## Known bugs and problem areas

no known bugs.


################################################

## 		Special Section 	      ##

################################################

## Describe your solution to the problem
Our solution to the problem involves a constraint where Democrats are not allowed to wait in line if Republicans are in line and vice versa. Since their is no constraint on who an Independent can vote with, if there is an open booth and no one in line to fill that booth, they will jump in and vote.  Once the opposing party is no longer in line or voting the party members that were waiting outside the line because they were 'finicky' and didn't want to vote with the opposing party, are filed into the line to vote. 

To simplify the explanation of this control scheme, consider the scenario where the first thread to enter the line is of the Democratic party. Members of the Democratic party that appear thereafter are allowed to enter the line and vote, and independent voters that register to vote will be able to jump to the next available voting booth. If a Republican voter attempts to enter the line they will have to wait until all Democrats have left the voting booth.  Once there are no longer democrats voting, the Republicans that are linedup waiting outside the voting area will be allowed to file in and start voting alongside Independents.



##  Describe how your solution meets the goals of this part of the project  ##

## How does your solution avoid deadlock?

Our solution avoids deadlock by using semaphores to control when each party has to wait to lineup to vote. Each has an equal amount of waits and posts to ensure our program doesn't hang when it's not supposed to. We also use mutexes to protect any sort of printing and counts to ensure safety from race conditions. 


## How does your solution prevent Republican and Democrat voters from waiting on a voting booth at the same time?
To prevent Democrats and Republicans from waiting on a voting booth at the same time we use two semaphores. Reps_wait_on_dems and dem_wait_on_reps.  We also have a counter for each party to increment if they are in line and decrement as they leave the vooting booth. The last of their party to leave the voting booth will signal to the opposing party that all of them who were waiting to get in line to vote can go ahead and file in.  At this time, the party that just signaled to the opposing party to go ahead and vote would now have to wait to be signaled back in line to vote. This back and forth allows for neither of the opposing finicky parties to be waiting in line/voting at the same time.  

Example: If a democrat is in line voting and a Republican wants to enter the line to vote it will have to wait via it's semaphore until it is released by the last democrat to leave the booth. If three Republicans were waiting via their semaphore to enter then three were signaled. 

## How is your solution fair? How are you defining "fairness" for this problem?

The fairness for Republicans and Democrats since they are finicky and don't want to stand next to eachother is that they get to go in waves. Once a wave of democrats vote then a wave of Republicans waiting to get in line get to move in and vice versa. Since Independents can vote with whichever party they want we decided that if there was an open booth and no one in line to fill it then an Independent could jump in. To avoid Independents filling up the booths too often (50 independents lineup to vote with a Democrat back at spot 51) we make the Independents wait if there are enough people in line to fill up the voting booths. So, if there are 10 booths and 10 people in line waiting to vote then an Independent would have to wait outside the line just as someone from the opposing party would via a semaphore and then would be signaled in when a spot in line opened up.  



## How does your solution prevent starvation? How are you defining "starvation" for this problem?

Starvation for this problem is defined as never getting to vote and keep getting pushed to the back of the line. We are preventing starvation by switching back and forth between parties who are voting and parties that are waiting to go in and vote. If your party is waiting outside the line to avoid the other party. When they leave, your wave that was waiting gets to move on in. Making Independents wait outside the line using a semaphore also helps with starvation because then they can't just jump in front of everyone all the time whenever they want. They only jump ahead when needed. 



