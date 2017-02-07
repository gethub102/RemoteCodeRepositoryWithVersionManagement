///////////////////////////////////////////////////////////////
// Cpp11-BlockingQueue.h - Thread-safe Blocking Queue        //
// ver 1.2                                                   //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016 //
//              Source from Dr. Jim                          //
/////////////////////////////////////////////////////////////// 
/*
* Package Operations:
* -------------------
* This package contains one thread-safe class: BlockingQueue<T>.
* Its purpose is to support sending messages between threads.
* It is implemented using C++11 threading constructs including
* std::condition_variable and std::mutex.  The underlying storage
* is provided by the non-thread-safe std::queue<T>.
*
* Required Files:
* ---------------
* BlockingQueue.h
*
* Build Process:
* --------------
* - devenv StringSocketServer.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.2 : 27 Feb 2016
* - added front();
* - added move ctor and move assignment
* - deleted copy ctor and copy assignment
* ver 1.1 : 26 Jan 2015
* - added copy constructor and assignment operator
* ver 1.0 : 03 Mar 2014
* - first release
*
*/



#ifdef TEST_BLOCKINGQUEUE
#include <condition_variable>
#include <mutex>
#include <thread>
#include <queue>
#include <string>
#include <iostream>
#include <sstream>
#include "BlockingQueue.h"
std::mutex ioLock;

void test(BlockingQueue<std::string>* pQ)
{
	std::string msg;
	do
	{
		msg = pQ->deQ();
		{
			std::lock_guard<std::mutex> l(ioLock);
			std::cout << "\n  thread deQed " << msg.c_str();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	} while (msg != "quit");
}

int main()
{
	std::cout << "\n  Demonstrating C++11 Blocking Queue";
	std::cout << "\n ====================================";

	BlockingQueue<std::string> q;
	std::thread t(test, &q);

	for (int i = 0; i<15; ++i)
	{
		std::ostringstream temp;
		temp << i;
		std::string msg = std::string("msg#") + temp.str();
		{
			std::lock_guard<std::mutex> l(ioLock);
			std::cout << "\n   main enQing " << msg.c_str();
		}
		q.enQ(msg);
		std::this_thread::sleep_for(std::chrono::milliseconds(3));
	}
	q.enQ("quit");
	t.join();

	std::cout << "\n";
	std::cout << "\n  Making move copy of BlockingQueue";
	std::cout << "\n -----------------------------------";

	std::string msg = "test";
	q.enQ(msg);
	std::cout << "\n  before move:";
	std::cout << "\n    q.size() = " << q.size();
	std::cout << "\n    q.front() = " << q.front();
	BlockingQueue<std::string> q2 = std::move(q);  // move assignment
	std::cout << "\n  after move:";
	std::cout << "\n    q2.size() = " << q2.size();
	std::cout << "\n    q.size() = " << q.size();
	std::cout << "\n    q2 element = " << q2.deQ() << "\n";


	std::cout << "\n\n";
}

#endif
