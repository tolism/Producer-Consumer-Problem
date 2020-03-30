# Producer-Consumer-Problem
Real Time Embedded Systems : 1st assignment

The producer-consumer problem using pthreads 


## Installation and Execution 

To execute the versions 
```
 gcc -o s prod-cons.c   -lpthread -lm  -std=gnu99 -std=c99 -O3
 ./s

```
The statistic file contains the three texts with the overall stastics of the runs and the python script that calculated them. To take fast the results from the simulations.c I used the following fish script

 function runtest
                                      for i in 1 2 4 8 16 
                                          for j in 1 2 4 8 16 32 64 128 
                                              echo Producers : $i Consumers $j 
                                              ./s $i $j >> ./stats1/q512L1000_"$i"_"$j".txt
                                              sleep 5
                                          end
                                      end
                                  end 
### Prerequisites

Pthread library.


### Statistic for LOOP = 50k and QUEUESIZE = 512 

Mean Waiting TIme | First Quarter | Second Quarter | Third Quarter | Fourth Quarter | Overall 
--- | --- | --- | --- |--- |--- 
Seconds | 301 | 283 | 290 | 286 | 289

## Author

* **Apostolos Moustaklis**  

