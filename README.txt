1. For CDS checking
  a. download the CDS checker
  b. put my CDS/ folder under model-chekcer/benchmarks/
  c. go to /moder-checker/benchmarks/CDS
  d. run $ make
  e. run $ ../run.sh ./newRWlock -y - f 10
 
 2. For unit test
  a. go to my test/ folder
  b. compile by running $ g++ rwlock.cc -o rwlock -std=c++11 -pthread 
  c. run the program & ./rwlock
  e. you could change the thread number by changing the glabal variable thrd_num, 
     and the read wriet percentage by veriable countr, countw.
  
