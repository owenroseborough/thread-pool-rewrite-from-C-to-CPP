#ifndef THREAD_DO_H
#define THREAD_DO_H

#include <condition_variable>
#include <vector>
#include <thread>
#include <any>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include "conc_queue.h"

using namespace std;

class thread_do {

private:
	shared_ptr<conc_queue<shared_ptr<function<void()>>>> job_queue;
	shared_ptr<mutex> num_threads_alive_mutex;
	shared_ptr<size_t> num_threads_alive;
	shared_ptr<size_t> num_threads;
	shared_ptr<condition_variable> all_threads_alive;
	shared_ptr<mutex> threads_keepalive_mutex;
	shared_ptr<bool> threads_keepalive;
	shared_ptr<mutex> job_queue_has_jobs_mutex;
	shared_ptr<mutex> num_threads_working_mutex;
	shared_ptr<size_t> num_threads_working;
	shared_ptr<bool> threads_all_idle;

public:
	thread_do(shared_ptr<conc_queue<shared_ptr<function<void()>>>> job_queue_param,
		shared_ptr<mutex> num_threads_alive_mutex_param,
		shared_ptr<size_t> num_threads_alive_param,
		shared_ptr<size_t> num_threads_param,
		shared_ptr<condition_variable> all_threads_alive_param,
		shared_ptr<mutex> threads_keepalive_mutex_param,
		shared_ptr<bool> threads_keepalive_param,
		shared_ptr<mutex> job_queue_has_jobs_mutex_param,
		shared_ptr<mutex> num_threads_working_mutex_param,
		shared_ptr<size_t> num_threads_working_param,
		shared_ptr<bool> threads_all_idle_param) {

		job_queue = job_queue_param;
		num_threads_alive_mutex = num_threads_alive_mutex_param;
		num_threads_alive = num_threads_alive_param;
		num_threads = num_threads_param;
		all_threads_alive = all_threads_alive_param;
		threads_keepalive_mutex = threads_keepalive_mutex_param;
		threads_keepalive = threads_keepalive_param;
		job_queue_has_jobs_mutex = job_queue_has_jobs_mutex_param;
		num_threads_working_mutex = num_threads_working_mutex_param;
		num_threads_working = num_threads_working_param;
		threads_all_idle = threads_all_idle_param;
	}
	void loop(){
		unique_lock<mutex> numThreadsAliveLock(*num_threads_alive_mutex);
		(*num_threads_alive)++;
		if (*num_threads_alive == *num_threads) {
			all_threads_alive->notify_one();
		}
		numThreadsAliveLock.unlock();
		

		unique_lock<mutex> keepAliveLock(*threads_keepalive_mutex);
		bool temp_threads_keepalive = *threads_keepalive;
		keepAliveLock.unlock();

		while (temp_threads_keepalive) {

			unique_lock<mutex> jobQueueHasJobsLock(*job_queue_has_jobs_mutex);
			//job_queue_has_jobs.wait(jobQueueHasJobsLock, !job_queue.empty());

			keepAliveLock.lock();
			temp_threads_keepalive = *threads_keepalive;
			keepAliveLock.unlock();

			if (temp_threads_keepalive) {

				unique_lock<mutex> numThreadsWorkingLock(*num_threads_working_mutex);
				if (*num_threads_working == 0) {
					*threads_all_idle = false;
				}
				(*num_threads_working)++;
				numThreadsWorkingLock.unlock();

				shared_ptr<function<void()>> job;
				job = *job_queue->try_pop();
				(*job)();  // Execute the function

				numThreadsWorkingLock.lock();
				(*num_threads_working)--;
				if (num_threads_working == 0) {
					*threads_all_idle = true;
				}
				numThreadsWorkingLock.unlock();
			}
		}
		numThreadsAliveLock.lock();
		(*num_threads_alive)--;
		numThreadsAliveLock.unlock();
	}

};
#endif
