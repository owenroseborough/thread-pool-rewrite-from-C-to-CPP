#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <vector>
#include <thread>
#include <any>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include "conc_queue.h"

using namespace std;

class thread_pool {

private:
	shared_ptr<size_t> num_threads;

	shared_ptr<bool> threads_keepalive;
	shared_ptr<mutex> threads_keepalive_mutex;

	bool threads_on_hold;
	shared_ptr<bool> threads_all_idle;    // may need to make this into condition variable in the future
	
	shared_ptr<size_t> num_threads_alive;
	shared_ptr<mutex> num_threads_alive_mutex;
	shared_ptr<condition_variable> all_threads_alive;

	shared_ptr<size_t> num_threads_working;
	shared_ptr<mutex> num_threads_working_mutex;
	shared_mutex num_threads_working_shared_mutex;

	shared_ptr<conc_queue<shared_ptr<function<void()>>>> job_queue;

	condition_variable job_queue_has_jobs;
	shared_ptr<mutex> job_queue_has_jobs_mutex;

	vector<thread> threads;

public:
	thread_pool(size_t);
	size_t get_num_threads_working();
	//void add_work(void*, void*, void*);

	// functionality to implement from the old C version:
	// int thpool_add_work(threadpool, void (*function_p)(void*), void* arg_p);
	// void thpool_wait(threadpool);
	// void thpool_pause(threadpool);
	// void thpool_resume(threadpool);
	// void thpool_destroy(threadpool);
};
#endif