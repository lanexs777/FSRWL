#include<thread>
#include<mutex>
#include<condition_variable>
#include "counter2.h"
using namespace std;

class rwLock {
	mutex mutex_;
	condition_variable condition;
	bool writeLocked;
	Combining_Tree* tree_lock;
	Combining_Tree* tree_release;


public:
	rwLock(int thrdNum) {
		writeLocked = false;
		tree_lock = new Combining_Tree(thrdNum);
		tree_release = new Combining_Tree(thrdNum);
	}

	size_t getReadNum() {
		return tree_lock->getResult();
	}

	void lockRead(int id) {
			
		unique_lock<mutex> monitor(mutex_);
		while(writeLocked) {
			condition.wait(monitor);
			//this_thread::yield();
		}
		tree_lock->getAndIncrement(id);
	}

	void unlockRead(int id) {
		
		unique_lock<mutex> monitor(mutex_);
		tree_release->getAndIncrement(id);
		if(tree_lock->getResult() == tree_release->getResult()) {
			condition.notify_all();
		}
	}

	void lockWrite() {
		unique_lock<mutex> monitor(mutex_);
		while(writeLocked) {
			condition.wait(monitor);
			//this_thread::yield();
		}
		writeLocked = true;
		while(tree_lock->getResult() != tree_release->getResult()) {
			condition.wait(monitor);
		}
	}

	void unlockWrite() {
		unique_lock<mutex> monitor(mutex_);
		writeLocked = false;
		condition.notify_all();
	}
};
