#include "pch.h"
#include "thread_pool.h"
#include "thread_do.h"

using namespace std;

thread_pool::thread_pool(size_t number_of_threads_desired) {

	num_threads = make_shared<size_t>(number_of_threads_desired);
	threads_keepalive = make_shared<bool>(true);
	threads_on_hold = false;
	threads_all_idle = make_shared<bool>(true);
	num_threads_alive = make_shared<size_t>(0);
	num_threads_working = make_shared<size_t>(0);
	job_queue = make_shared<conc_queue<shared_ptr<function<void()>>>>();

	num_threads_alive_mutex = make_shared<mutex>();
	threads_keepalive_mutex = make_shared<mutex>();
	job_queue_has_jobs_mutex = make_shared<mutex>();
	num_threads_working_mutex = make_shared<mutex>();
	all_threads_alive = make_shared<condition_variable>();

	for (size_t i = 0; i < *num_threads; i++) {
		thread_do* thread_loop = new thread_do(job_queue, num_threads_alive_mutex, num_threads_alive,
			num_threads, all_threads_alive, threads_keepalive_mutex, threads_keepalive, job_queue_has_jobs_mutex,
			num_threads_working_mutex, num_threads_working, threads_all_idle);

		threads.push_back(thread(&thread_do::loop, &*thread_loop));
		delete thread_loop;
	}

	unique_lock<mutex> numThreadsAliveLock(*num_threads_alive_mutex);
	all_threads_alive->wait(numThreadsAliveLock, [this] { return num_threads_alive == num_threads; });
}
size_t thread_pool::get_num_threads_working() {
	shared_lock<shared_mutex> numThreadsWorkingSharedLock(num_threads_working_shared_mutex);
	return *num_threads_working;
}

//void thread_pool::add_work(void* function, void* arg1, void* arg2) {
//	// need to make variadic function
//	job_queue.push(std::make_shared<std::function<void()>>(std::bind(add, 3, 7)));

//}
int main() {
	return 0;
}