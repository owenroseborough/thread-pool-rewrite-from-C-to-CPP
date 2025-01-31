#include "thread_pool.h"

using namespace std;

thread_pool::thread_pool(size_t num_threads) {

	threads_keepalive = true;
	threads_on_hold = false;
	threads_all_idle = true;
	num_threads_alive = 0;
	num_threads_working = 0;
	job_queue = make_shared<conc_queue>();
	threads = make_unique<vector<thread>>();

	for (size_t i = 0; i < num_threads; i++) {
		threads->push_back(thread(thread_do));
	}

	// busy wait below, might try to make better
	/* Wait for threads to initialize */
	while (thpool_p->num_threads_alive != num_threads) {}
}
void thread_pool::thread_do() {

	unique_lock<mutex> lk1(num_threads_alive_mutex);
	num_threads_alive++;
	lk1.unlock();
	
	unique_lock<mutex> lk2(threads_keepalive_mutex);
	bool temp_threads_keepalive = threads_keepalive;
	lk2.unlock();

	while (temp_threads_keepalive) {

		// replace line below with condition variable
		bsem_wait(thpool_p->jobqueue.has_jobs);

		if (threads_keepalive) {

			pthread_mutex_lock(&thpool_p->thcount_lock);
			thpool_p->num_threads_working++;
			pthread_mutex_unlock(&thpool_p->thcount_lock);

			/* Read job from queue and execute it */
			void (*func_buff)(void*);
			void* arg_buff;
			job* job_p = jobqueue_pull(&thpool_p->jobqueue);
			if (job_p) {
				func_buff = job_p->function;
				arg_buff = job_p->arg;
				func_buff(arg_buff);
				free(job_p);
			}

			pthread_mutex_lock(&thpool_p->thcount_lock);
			thpool_p->num_threads_working--;
			if (!thpool_p->num_threads_working) {
				pthread_cond_signal(&thpool_p->threads_all_idle);
			}
			pthread_mutex_unlock(&thpool_p->thcount_lock);

		}
	}
	pthread_mutex_lock(&thpool_p->thcount_lock);
	thpool_p->num_threads_alive--;
	pthread_mutex_unlock(&thpool_p->thcount_lock);

	return NULL;
}