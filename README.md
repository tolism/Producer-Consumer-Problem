# Producer-Consumer-Problem
Real Time Embedded Systems : 1st assignment

The producer-consumer problem using pthreads 


## Installation and Execution 

To execute the versions 
```
 gcc -o s prod-cons.c   -lpthread -lm  -std=gnu99 -std=c99 -O3
 ./s

```
The statistics file contains the three texts with the overall stastics of the runs and the python script that calculated them. To take fast the results from the simulations.c I used the fish shell function inside the folder , which executes for combinations of different producers / consumers and saves the results at the txt's 

### Prerequisites

Pthread library.


### Some Waiting Times (nanosec)  for LOOP = 50k and QUEUESIZE = 512 

Mean Waiting TIme | First Quarter | Second Quarter | Third Quarter | Fourth Quarter | Overall 
--- | --- | --- | --- |--- |--- 
P:1 Q:8 | 2337.312 | 2229.378 | 2038.541 | 2081.723 | 2171.739
P:2 Q:32 | 1947.161 | 1703.986 | 1694.448 | 1677.839 | 1755.859
P:8 Q:1 | 1947.161 | 1703.986 | 1694.448 | 1677.839 | 1755.859
P:2 Q:32 | 1488797.723 | 1441082.254 | 1438194.836 | 1582564.106 | 1585648.512
P:16 Q:32 | 2723.709 | 2966.451 | 2993.526 | 2924.595 | 2902.072





## Author

* **Apostolos Moustaklis**  

