# Producer-Consumer-Problem
Real Time Embedded Systems : 1st assignment

The producer-consumer problem using pthreads 


## Installation and Execution 

To execute the versions 
```
 gcc -o s prod-cons.c   -lpthread -lm  -std=gnu99 -std=c99 -O3
 ./s

```
The statistic file contains the three texts with the overall stastics of the runs and the python script that calculated them. To take fast the results from the simulations.c I used the fish shell function inside the folder , which executes for combinations of different producers / consumers and saves the results at the txt's 

### Prerequisites

Pthread library.


### Statistic for LOOP = 50k and QUEUESIZE = 512 

Mean Waiting TIme | First Quarter | Second Quarter | Third Quarter | Fourth Quarter | Overall 
--- | --- | --- | --- |--- |--- 
Seconds | 301 | 283 | 290 | 286 | 289

## Author

* **Apostolos Moustaklis**  

