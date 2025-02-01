#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include "conc_queue.h"

class thread_pool {

private:
	size_t num_threads;

	bool threads_keepalive;
	mutex threads_keepalive_mutex;

	bool threads_on_hold;
	bool threads_all_idle;    // may need to make this into condition variable in the future
	
	size_t num_threads_alive;
	mutex num_threads_alive_mutex;
	condition_variable all_threads_alive;

	size_t num_threads_working;
	mutex num_threads_working_mutex;
	shared_mutex num_threads_working_shared_mutex;

	shared_ptr<conc_queue<function<void()>>> job_queue;
	condition_variable job_queue_has_jobs;
	mutex job_queue_has_jobs_mutex;

	unique_ptr<vector<thread>> threads;

public:
	thread_pool(size_t);
	void thread_do();
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