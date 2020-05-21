#include<iostream>
#include<thread>
#include "rwLock.h"
#include<time.h>
#include<stdio.h>
rwLock* lock_;
int total;
long long countr = 1000000;
int countw = 10000;
int thrd_num = 4;

void writeJob() {
	for(int i=0;i<countw;i++) {
		lock_->lockWrite();
		total += 1;
		//cout<< "thread" << this_thread::get_id() << " add to " <<total<<endl;
		lock_->unlockWrite();
	}
}

void readJob(int id) {
	while(lock_->getReadNum() <= countr) {
		lock_->lockRead(id);
		//cout<<lock_->getReadNum()<<endl;
		//cout<<"current total :"<<total<<endl;
		lock_->unlockRead(id);
	};
}

int main() {
	total = 0;
	
	for(int k=1;k<=thrd_num;k++) {

		int thread_num = 1<<k;
		lock_ = new rwLock(thread_num - 1);
	
		clock_t b, e, a, v;

		thread thrd[thread_num];
		b = clock();
	
		for(int j=0;j<thread_num;j++)
			thrd[j] = thread(readJob, j);
	
		// uncokmment this to do writeJob
		thread thrdW;
		thrdW = thread(writeJob);
		thrdW.join();
	
		for(int i=0;i<thread_num;i++)
			thrd[i].join();
		e = clock();
		int tn = 1 <<(k-1);
		cout<<"thread number: "<<tn<<endl;
		cout<<"Final total ="<<total<<endl;
		printf("elpased time %.4f\n", (double)(e-b)/CLOCKS_PER_SEC);
		total = 0;	
	}

	return 0;
	
}
