#include<mutex>
#include<condition_variable>
#include<iostream>
#include "counter.h"
using namespace std;

class rwLock {
	mutex mutex_;
	condition_variable condition;
	bool writeLocked;
	size_t readLockNum;
	size_t readReleaseNum;
	
	// scalable counter
	Combining_Tree* tree_lock;
	Combining_Tree* tree_release;

public:
	rwLock(int thrdNum) {
		writeLocked = false;
		readLockNum = 0;
		readReleaseNum = 0;
		tree_lock = new Combining_Tree(thrdNum);
		tree_release = new Combining_Tree(thrdNum);
	}
	size_t getReadNum() {
		return readLockNum;
	}

	void lockRead(int id) {
		//cout<<"in lockRead"<<endl;
		mutex_.lock();
		while(writeLocked) {
			//cout<<"wait for write finished" <<endl;
			condition.wait(mutex_);
		}
		mutex_.unlock();
		
		//readLockNum++;
		tree_lock->getAndIncrement(id);
		//cout<<"readlock++"<<endl;
		//cout<<"finish lockRaed"<<endl;
	}

	void unlockRead(int id) {
		//cout<<"in unlockRead"<<endl;
		tree_release->getAndIncrement(id);
		mutex_.lock();
		//readReleaseNum++;
		//if(readLockNum == readReleaseNum) {
		if(tree_lock->getResult() == tree_release->getResult()) {
			condition.notify_all();
		}
		mutex_.unlock();
		//cout<<"finished unlockRaed"<<endl;
	}

	void lockWrite() {
		//cout<<"in lockWrite"<<endl;
		mutex_.lock();
		while(writeLocked) {
			condition.wait(mutex_);
		}
		writeLocked = true;
		//while(readLockNum != readReleaseNum) {
		//cout<<"lock Num: "<<tree_lock->getResult()<<" release Num: "<<tree_release->getResult()<<endl;
		while(tree_lock->getResult() != tree_release->getResult()) {
			condition.wait(mutex_);
		}
		mutex_.unlock();
		//cout<<"finished lockWrite"<<endl;
	}

	void unlockWrite() {
		//cout<<"in unlockWrite"<<endl;
		writeLocked = false;
		condition.notify_all();
		//cout<<"finished unlockWQrite"<<endl;
	}
};
