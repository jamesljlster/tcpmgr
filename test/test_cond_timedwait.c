#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void* lock_holder(void* arg)
{
	int ret;

	ret = pthread_mutex_lock(arg);
	if(ret != 0)
	{
		printf("%s, %d: pthread_mutex_lock() failed with error: %d\n", __FUNCTION__,
				__LINE__, ret);
	}

	sleep(5);

	ret = pthread_mutex_unlock(arg);
	if(ret != 0)
	{
		printf("%s, %d: pthread_mutex_unlock() failed with error: %d\n", __FUNCTION__,
				__LINE__, ret);
	}

	pthread_exit(NULL);
	return NULL;
}

int main()
{
	int ret;

	pthread_t th;
	pthread_mutex_t mutex;
	pthread_mutexattr_t mutexAttr;
	pthread_cond_t cond;

	struct timespec timeout;

	// Initial mutex and cond
	ret = pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_ERRORCHECK);
	if(ret != 0)
	{
		printf("%d: pthread_mutexattr_settype() failed with error: %d\n", __LINE__, ret);
		return -1;
	}

	ret = pthread_mutex_init(&mutex, &mutexAttr);
	if(ret != 0)
	{
		printf("%d: pthread_mutex_init() failed with error: %d\n", __LINE__, ret);
		return -1;
	}

	ret = pthread_cond_init(&cond, NULL);
	if(ret != 0)
	{
		printf("%d: pthread_cond_init() failed with error: %d\n", __LINE__, ret);
		return -1;
	}

	// Test with condition timeout
	printf("\n=== Test condition timeout ===\n");
	ret = pthread_mutex_lock(&mutex);
	if(ret != 0)
	{
		printf("%d: pthread_mutex_lock() failed with error: %d\n", __LINE__, ret);
	}

	clock_gettime(CLOCK_REALTIME, &timeout);
	timeout.tv_sec += 1;
	ret = pthread_cond_timedwait(&cond, &mutex, &timeout);
	if(ret != 0)
	{
		printf("%d: pthread_cond_timedwait() failed with error: %d\n", __LINE__, ret);
	}

	ret = pthread_mutex_trylock(&mutex);
	if(ret != 0)
	{
		printf("%d: pthread_mutex_trylock() failed with error: %d\n", __LINE__, ret);
	}

	// Test with condition timeout and mutex lock failed
	printf("\n=== Test condition timeout and mutex lock failed ===\n");
	ret = pthread_mutex_unlock(&mutex);
	if(ret != 0)
	{
		printf("%d: pthread_mutex_unlock() failed with error: %d\n", __LINE__, ret);
	}

	ret = pthread_create(&th, NULL, lock_holder, &mutex);
	if(ret != 0)
	{
		printf("%d: pthread_create() failed with error: %d\n", __LINE__, ret);
	}

	sleep(3);

	clock_gettime(CLOCK_REALTIME, &timeout);
	timeout.tv_sec += 1;
	ret = pthread_cond_timedwait(&cond, &mutex, &timeout);
	if(ret != 0)
	{
		printf("%d: pthread_cond_timedwait() failed with error: %d\n", __LINE__, ret);
	}

	ret = pthread_mutex_trylock(&mutex);
	if(ret != 0)
	{
		printf("%d: pthread_mutex_trylock() failed with error: %d\n", __LINE__, ret);
	}

	ret = pthread_join(th, NULL);
	if(ret != 0)
	{
		printf("%d: pthread_join() failed with error: %d\n", __LINE__, ret);
	}

	ret = pthread_mutex_trylock(&mutex);
	if(ret != 0)
	{
		printf("%d: pthread_mutex_trylock() failed with error: %d\n", __LINE__, ret);
	}

	return 0;
}
