#include<iostream>
#include<stdio.h>
#include<atomic>
#include<mutex>
#include<vector>
#include<ostream>
#include<condition_variable>
using namespace std;
enum CStatus {
	IDLE,
	FIRST,
	SECOND,
	RESULT,
	ROOT
};
class Node {
friend   class Combining_Tree;
public:
	bool locked;
	enum CStatus cStatus;
	int id, firstValue, secondValue, result;
	Node* parent;
	condition_variable cv;
	mutex lock;

	Node() : locked(false), cStatus(ROOT), firstValue(0), secondValue(0), result(0), parent(NULL) {}
	Node(Node * myParent) : locked(false), cStatus(IDLE), firstValue(0), secondValue(0), result(0), parent(myParent) {}
	Node* getParent() { return parent; }

	bool precombine() {
		//cout<<"in precombine"<<endl;
		//unique_lock<mutex> l(lock);
		lock.lock();
		//while (locked) cv.wait(l);
		while (locked) cv.wait(lock);
		switch (cStatus) {
		case IDLE:
			cStatus = FIRST;
			lock.unlock();
			return true;
		case FIRST:
			locked = true;
			cStatus = SECOND;
			lock.unlock();
			return false;
		case ROOT:
			lock.unlock();
			return false;
		default:
			printf("error in precombine");
			exit(1);
		}
		lock.unlock();
	}

	int combine(int combined) {
		//cout<<"in combine"<<endl;
		//unique_lock<mutex> l(lock);
		//while (locked) cv.wait(l);
		lock.lock();
		while (locked) cv.wait(lock);
		firstValue = combined;
		locked = true;
		switch (cStatus) {
		case FIRST:
			lock.unlock();
			return  firstValue;
		case SECOND:
			lock.unlock();
			return  firstValue + secondValue;
		default:
			printf("error in combine");
			exit(1);
		}
		lock.unlock();
	}

	int operation(int combined) {
		//cout<<"in op"<<endl;
		//unique_lock<mutex> l(lock);
		lock.lock();
		int oldValue;
		switch (cStatus) {
		case ROOT:
			oldValue = result;
			result += combined;
			lock.unlock();
			return oldValue;
		case SECOND:
			secondValue = combined;
			locked = false;
			cv.notify_all();
			//while (cStatus != RESULT) cv.wait(l);
			while (cStatus != RESULT) cv.wait(lock);
			locked = false;
			cv.notify_all();
			cStatus = IDLE;
			lock.unlock();
			return result;
		default:
			printf("error in operation");
			exit(1);
		}
		lock.unlock();
	}

	void distribute(int prior) {
		//cout<<"in dis"<<endl;
		//unique_lock<mutex> l(lock);
		lock.lock();
		switch (cStatus) {
		case FIRST:
			cStatus = IDLE;
			locked = false;
			break;
		case SECOND:
			result = prior + firstValue;
			cStatus = RESULT;
			break;
		default:
			printf("error in distribute");
			exit(1);
		}
		cv.notify_all();
		lock.unlock();
	}
};
class Combining_Tree {
public:
	Node **leaf, **nodes;
	int num_tree;
	Combining_Tree(int size) {
		num_tree = size;
		int num_leaf = (num_tree + 1) / 2; 
		leaf = new Node*[num_leaf];
		nodes = new Node*[num_tree];
		nodes[0] = new Node();
		nodes[0]->id = 1;
		for (int i = 1; i < num_tree; i++) {
			nodes[i] = new Node(nodes[(i - 1) / 2]);
			nodes[i]->id = i + 1;
		}

		for (int i = 0; i < num_leaf; i++) {
			leaf[i] = nodes[num_tree - num_leaf + i];
		}
	}
	
	int getResult() {
		return nodes[0]->result;
	}
	int getAndIncrement(int thread_id) {
		Node *myLeaf = leaf[thread_id / 2];

		Node *node = myLeaf;
		while (node->precombine()) {
			node = node->getParent();
		}
		Node *stop = node;
		node = myLeaf;

		int combined = 1;
		vector<Node*> s;
		while (node != stop) {
			combined = node->combine(combined);
			s.push_back(node);
			node = node->getParent(); 
		}

		int prior = stop->operation(combined);
		while (!s.empty()) { 
			node = s.back();
			s.pop_back();
			node->distribute(prior); 
		}
		return prior;
	}
};

