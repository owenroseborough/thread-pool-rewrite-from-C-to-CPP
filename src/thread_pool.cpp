#include "thread_pool.h"

using namespace std;

thread_pool::thread_pool(size_t number_of_threads_desired) {

	num_threads = number_of_threads_desired;
	threads_keepalive = true;
	threads_on_hold = false;
	threads_all_idle = true;
	num_threads_alive = 0;
	num_threads_working = 0;
	//job_queue = make_shared<conc_queue<any>>();

	for (size_t i = 0; i < num_threads; i++) {
		threads.push_back(thread(&thread_pool::thread_do, this));
	}

	unique_lock<mutex> numThreadsAliveLock(num_threads_alive_mutex);
	all_threads_alive.wait(numThreadsAliveLock, [this] { return num_threads_alive == num_threads; });
}
void thread_pool::thread_do() {

	size_t temp_num_threads_alive;
	unique_lock<mutex> numThreadsAliveLock(num_threads_alive_mutex);
	num_threads_alive++;
	temp_num_threads_alive = num_threads_alive;
	numThreadsAliveLock.unlock();
	if (temp_num_threads_alive == num_threads) {
		all_threads_alive.notify_one();
	}
	
	unique_lock<mutex> keepAliveLock(threads_keepalive_mutex);
	bool temp_threads_keepalive = threads_keepalive;
	keepAliveLock.unlock();

	while (temp_threads_keepalive) {

		unique_lock<mutex> jobQueueHasJobsLock(job_queue_has_jobs_mutex);
		//job_queue_has_jobs.wait(jobQueueHasJobsLock, !job_queue.empty());

		keepAliveLock.lock();
		temp_threads_keepalive = threads_keepalive;
		keepAliveLock.unlock();

		if (temp_threads_keepalive) {

			unique_lock<mutex> numThreadsWorkingLock(num_threads_working_mutex);
			if (num_threads_working == 0) {
				threads_all_idle = false;
			}
			num_threads_working++;
			numThreadsWorkingLock.unlock();

			shared_ptr<function<void()>> job;
			job = *job_queue->try_pop();
			(*job)();  // Execute the function

			numThreadsWorkingLock.lock();
			num_threads_working--;
			if (num_threads_working == 0) {
				threads_all_idle = true;
			}
			numThreadsWorkingLock.unlock();
		}
	}
	numThreadsAliveLock.lock();
	num_threads_alive--;
	numThreadsAliveLock.unlock();
}
size_t thread_pool::get_num_threads_working() {
	shared_lock<shared_mutex> numThreadsWorkingSharedLock(num_threads_working_shared_mutex);
	return num_threads_working;
}

//void thread_pool::add_work(void* function, void* arg1, void* arg2) {
//	// need to make variadic function
//	job_queue.push(std::make_shared<std::function<void()>>(std::bind(add, 3, 7)));

//}
int main() {
	return 0;
}