# CS441/541 Project 3 - Part 1

## Author(s):

Christa Brehm and Adam Geipel


## Date:

3/9/2016


## Description:

Bounded-buffer producer-consumer producer.  A consumer can only consumer when the producer has produced items to consume. 
The producer must not overwrite the consumer.


## How to build the software

run the makefile


## How to use the software

The program takes four inputs (fourth is optional). Time to live (required), number of producers (required), number of consumers (required), buffer size (optional, defaults to 10 if not specified).   EXAMPLE: ./bounded-buffer 5 2 1 3     time to live = 5 seconds, number of producers = 2, number of consumers = 1, and the buffer size = 3.


## How the software was tested

The software was tested with many different numbers as input.  We experienced deadlock situations when there was a smaller buffersize say < 5 and a longer time to live > 15 seconds.  A higher probability of deadlock in this situation would make sense because the producer has a higher chance of catching up with the consumer.  Majority of the time the software works just fine with the next produced is also the next consumed but towards the end, with a small buffersize, you may see deadlock. Also, the more drastic the difference in producer vs. consumer threads the more likely it seems deadlock shows up.

Seems to be the same even when hardcoding usleep()to be different for producer and consumer.

## Known bugs and problem areas

As described above, sometimes we encountered a deadlock situation. 
Buffer Size                :    5
Time To Live (seconds)     :   15
Number of Producer threads :    1
Number of Consumer threads :   10
-------------------------------------
Initial Buffer:                                         [ -1^v  -1  -1  -1  -1 ]
Producer  0: Total      1, Value        8               [  8v  -1^  -1  -1  -1 ]
Consumer  0: Total      1, Value        8               [ -1  -1^v  -1  -1  -1 ]
Producer  0: Total      2, Value        2               [ -1   2v  -1^  -1  -1 ]
Consumer  2: Total      2, Value        2               [ -1  -1  -1^v  -1  -1 ]
Producer  0: Total      3, Value        0               [ -1  -1   0v  -1^  -1 ]
Consumer  4: Total      3, Value        0               [ -1  -1  -1  -1^v  -1 ]
Producer  0: Total      4, Value        9               [ -1  -1  -1   9v  -1^ ]
Consumer  3: Total      4, Value        9               [ -1  -1  -1  -1  -1^v ]
Producer  0: Total      5, Value        6               [ -1^  -1  -1  -1   6v ]
Consumer  6: Total      5, Value        6               [ -1^v  -1  -1  -1  -1 ]
Producer  0: Total      6, Value        9               [  9v  -1^  -1  -1  -1 ]
Consumer  1: Total      6, Value        9               [ -1  -1^v  -1  -1  -1 ]
Producer  0: Total      7, Value        3               [ -1   3v  -1^  -1  -1 ]
Consumer  5: Total      7, Value        3               [ -1  -1  -1^v  -1  -1 ]
Producer  0: Total      8, Value        8               [ -1  -1   8v  -1^  -1 ]
Consumer  8: Total      8, Value        8               [ -1  -1  -1  -1^v  -1 ]
Producer  0: Total      9, Value        2               [ -1  -1  -1   2v  -1^ ]
Consumer  7: Total      9, Value        2               [ -1  -1  -1  -1  -1^v ]
Producer  0: Total      10, Value       7               [ -1^  -1  -1  -1   7v ]
Consumer  9: Total      10, Value       7               [ -1^v  -1  -1  -1  -1 ]
Producer  0: Total      11, Value       9               [  9v  -1^  -1  -1  -1 ]
Consumer  0: Total      11, Value       9               [ -1  -1^v  -1  -1  -1 ]
Producer  0: Total      12, Value       1               [ -1   1v  -1^  -1  -1 ]
Consumer  2: Total      12, Value       1               [ -1  -1  -1^v  -1  -1 ]
Producer  0: Total      13, Value       7               [ -1  -1   7v  -1^  -1 ]
Consumer  4: Total      13, Value       7               [ -1  -1  -1  -1^v  -1 ]
Producer  0: Total      14, Value       4               [ -1  -1  -1   4v  -1^ ]
Consumer  3: Total      14, Value       4               [ -1  -1  -1  -1  -1^v ]
Producer  0: Total      15, Value       2               [ -1^  -1  -1  -1   2v ]
Consumer  6: Total      15, Value       2               [ -1^v  -1  -1  -1  -1 ]
Producer  0: Total      16, Value       0               [  0v  -1^  -1  -1  -1 ]
Consumer  1: Total      16, Value       0               [ -1  -1^v  -1  -1  -1 ]
Producer  0: Total      17, Value       2               [ -1   2v  -1^  -1  -1 ]
Consumer  5: Total      17, Value       2               [ -1  -1  -1^v  -1  -1 ]
^C
