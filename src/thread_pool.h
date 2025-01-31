#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <vector>
#include <thread>
#include <mutex>
#include "conc_queue.h"

class thread_pool {

private:
	// old C variables for thread pool struct
	//thread** threads;                  /* pointer to threads        */
	// volatile int num_threads_alive;      /* threads currently alive   */
	// volatile int num_threads_working;    /* threads currently working */
	// pthread_mutex_t  thcount_lock;       /* used for thread count etc */
	// pthread_cond_t  threads_all_idle;    /* signal to thpool_wait     */
	// jobqueue  jobqueue;                  /* job queue                 */

	// new C++ variables
	bool threads_keepalive;
	mutex threads_keepalive_mutex;
	bool threads_on_hold;
	bool threads_all_idle;
	size_t num_threads_alive;
	mutex num_threads_alive_mutex;
	size_t num_threads_working;
	shared_ptr<conc_queue> job_queue;
	unique_ptr<vector<thread>> threads;


public:
	thread_pool(size_t num_threads);
	void thread_do();

	// functionality to implement from the old C version:
	// int thpool_add_work(threadpool, void (*function_p)(void*), void* arg_p);
	// void thpool_wait(threadpool);
	// void thpool_pause(threadpool);
	// void thpool_resume(threadpool);
	// void thpool_destroy(threadpool);
	// int thpool_num_threads_working(threadpool);


};
#endif