from statistics import median
import numpy as np

producers = [ 1 ,2 , 4 , 8 , 16]
consumers = [ 1 , 2 ,4 , 8 , 16 , 32 , 64 , 128]




def results(prodI , consJ):
    onoma = "q512L1000_"+str(i)+"_"+str(j)+".txt"
    with open(onoma) as f:
        v0 = f.read().splitlines()


    exclude = ["," , ":" ]
    #exclude = ["," , ":" ]
    result =[ i for i in v0 if not any([e for e in exclude if e in i])]

    V0 =list(map(float,result))

    print("Getting statistics from file :  " , onoma)
    splitted = np.array_split(V0, 4)
    print("Calculating the mean for the first quarter")
    print("Mean : " , np.mean(splitted[0] ,  dtype=np.float64) )
    print("Calculating the mean for the second quarter")
    print("Mean : " , np.mean(splitted[1] ,  dtype=np.float64) )
    print("Calculating the mean for the third quarter")
    print("Mean : " , np.mean(splitted[2] ,  dtype=np.float64) )
    print("Calculating the mean for the forth quarter")
    print("Mean : " , np.mean(splitted[3] ,  dtype=np.float64) )

    print("Calculating statistics from all the data")



    mm =  np.mean(V0 ,  dtype=np.float64)
    print("Mean in usec : " ,mm/1000)
    print("Median : "  , np.median(V0) )
    print("Min : " , np.amin(V0) )
    print("Max : " , np.amax(V0) )
    stdDev = np.std(V0)
    print("Std : " , stdDev )
    print("CV is :" ,stdDev / mm)

for i in producers:
    print(i)
    for j in consumers:
        results(i,j)
