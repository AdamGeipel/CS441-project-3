# CS441/541 Project 3 - Part 1

## Author(s):

Christa Brehm and Adam Geipel


## Date:

March 24th, 2016


## Description:

The finicky-voter is a program that simulates the operation of a polling booth with a finite number of voting booths and voters from the Republican, Democrat, and Independent parties.

After two seconds of waiting for voter threads to generate, voters are let into the polling station. Once inside, the voters wait to register, register, then get in line and wait for a voting booth. Only one of the two partisan parties and independents can wait on and vote at a booth at a time.

Voter threads sleep when registering and when voting to simulate filling out registration paperwork and taking time to thoughtfully vote.


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

Software was tested by running the code multiple times with different voting conditions.

Debug statements were added before and after each critical section to determine if threads made it through.


## Known bugs and problem areas

TODO


################################################

## 		Special Section 	      ##

################################################

## Describe your solution to the problem

Our solution to the problem involves a constraint on the Democrat and Republican parties such that consecutive threads from one partisan party and any independents are allowed to line up until a thread from the second party attempts to line up. At which point, all members of the party in line vote and allow the next party to line up and vote. This process continues cyclically until all voters have gone through the polls.

To simplify the explanation of this control scheme, consider the scenario where the first thread to enter the line is of the Democratic party. Members of the Democratic party that appear thereafter are allowed to enter the line and vote, and independent voters that register to vote will be able to jump to the next available voting booth.

If a Republican voter attempts to register, the Democratic voters will exit the line, vote, and exit the polling station, thus allowing the Republican to enter the line as it becomes clear of Democrats. The process then repeats with the Republican party in line to vote.



##  Describe how your solution meets the goals of this part of the project  ##

## How does your solution avoid deadlock?

Our solution avoids deadlock by 


## How does your solution prevent Republican and Democrat voters from waiting on a voting booth at the same time?

Our solution prevents Republican and Democrat voters from waiting on a voting booth at the same time  by using a counter to check if a member of the other party is in line. If at least one member of the other party is in line, the intruding party member will increment its own counter, preventing any more members of the currently voting party from entering the line. 

The intruding thread will then be forced to wait outside the line via a semaphore barrier until the voting party has completed voting. 


## How is your solution fair? How are you defining "fairness" for this problem?

Our solutin is fair because it allows Democrats and Republicans to interrupt each other in order to ensure each party may vote.



## How does your solution prevent starvation? How are you defining "starvation" for this problem?

Starvation for this problem is defined as



