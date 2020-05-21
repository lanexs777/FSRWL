#include<iostream>
#include<threads.h>
#include "newRWlock.h"
#include "librace.h"
#include<time.h>
#include<stdio.h>

rwLock* lock_;
int total;
int count = 2;
int thrdNum = 1; 

void writeJob(void* id) {
	for(int i=0;i<count;i++) {
		lock_->lockWrite();
		total += 1;
		cout<< "thread " << *(int*)id << " add to " <<total<<endl;
		lock_->unlockWrite();
	}
}

void readJob(void* id) {
	for(int i=0;i<count;i++) {
		lock_->lockRead(*(int*)id);
		cout<<"thread "<<*(int*)id<<" read current total :"<<total<<endl;
		lock_->unlockRead(*(int*)id);
	}
}



int user_main(int argc, char** argv) {
	total = 0;
	
	int thread_num = 1<<thrdNum;
	lock_ = new rwLock(thread_num - 1);
	thrd_t thrd[5];
	
	int i;
	
	clock_t b, e;
	b = clock();
	
	for(i=0;i<1;i++) {
		int* in = (int*) malloc(sizeof(int));
		*in = i;
		thrd_create(&thrd[i], &readJob, in);
	}
	
	for(int i=1;i<2;i++) {
		int *in = (int*) malloc(sizeof(int));
		*in = i;
		thrd_create(&thrd[i], &writeJob, in);
	}

	for(int i=0;i<2;i++) 
		thrd_join(thrd[i]);
	
	e = clock();
	cout<<"Final total ="<<total<<endl;
	printf("elapsed time: %.4f\n", (double)(e - b)/ CLOCKS_PER_SEC);

	return 0;
	
}
