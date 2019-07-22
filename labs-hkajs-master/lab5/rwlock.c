#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <assert.h>
#include "rwlock.h"
/* rwl implements a reader-writer lock.
 * A reader-write lock can be acquired in two different modes, 
 * the "read" (also referred to as "shared") mode,
 * and the "write" (also referred to as "exclusive") mode.
 * Many threads can grab the lock in the "read" mode.  
 * By contrast, if one thread has acquired the lock in "write" mode, no other 
 * threads can acquire the lock in either "read" or "write" mode.
 */

//helper function
static inline int
cond_timedwait(pthread_cond_t *c, pthread_mutex_t *m, const struct timespec *expire)
{
	int r; 
	if (expire != NULL)  {
		r = pthread_cond_timedwait(c, m, expire);
	} else
		r = pthread_cond_wait(c, m);
	assert(r == 0 || r == ETIMEDOUT);
       	return r;
}

//rwl_init initializes the reader-writer lock 
void
rwl_init(rwl *l)
{
	l->r_waiters = 0;	
	l->w_waiters = 0;
	l->readers = 0;
	l->writer = 0;
	pthread_mutex_init(&l->mutex,NULL);
	pthread_cond_init(&l->wcond,NULL);
	pthread_cond_init(&l->rcond,NULL);
}

//rwl_nwaiters returns the number of threads *waiting* to acquire the lock
//Note: it should not include any thread who has already grabbed the lock
int
rwl_nwaiters(rwl *l) 
{
	return (l->r_waiters) + (l->w_waiters);
}

//rwl_rlock attempts to grab the lock in "read" mode
//if lock is not grabbed before absolute time "expire", it returns ETIMEDOUT
//else it returns 0 (when successfully grabbing the lock)
int
rwl_rlock(rwl *l, const struct timespec *expire)
{
	//pthread has if already inside, so more efficient
	pthread_mutex_lock(&l->mutex);
	l->r_waiters++;
	while(l->writer != 0 || l->w_waiters != 0){
		if(cond_timedwait(&l->rcond,&l->mutex,expire) == 0){
			l->readers++;
			l->r_waiters--;
			pthread_mutex_unlock(&l->mutex);
			return 0;
		}
		if(cond_timedwait(&l->rcond,&l->mutex,expire) == ETIMEDOUT){
			l->r_waiters--;
			pthread_mutex_unlock(&l->mutex);
			return ETIMEDOUT;
		}
	}
	l->r_waiters--;
	pthread_mutex_unlock(&l->mutex);
	(l->readers)++;
	return 0;
}

//rwl_runlock unlocks the lock held in the "read" mode
void
rwl_runlock(rwl *l)
{
	//broadcast is the same as signal for wcond because there can only
	//be one writer.
	//I wonder if it is slower though?
	pthread_mutex_lock(&l->mutex);
	l->readers--;
	pthread_cond_broadcast(&l->wcond);
	pthread_mutex_unlock(&l->mutex);
}

//rwl_wlock attempts to grab the lock in "write" mode
//if lock is not grabbed before absolute time "expire", it returns ETIMEDOUT
//else it returns 0 (when successfully grabbing the lock)
int
rwl_wlock(rwl *l, const struct timespec *expire)
{
	pthread_mutex_lock(&l->mutex);
	l->w_waiters++;
	//weirdly enough using l->readers > 0 deadlocks resize?
	while (l->writer == 1 || l->readers != 0){
		if (cond_timedwait(&l->wcond,&l->mutex,expire) == 0){
			l->w_waiters--; 
			l->writer = 1;
			pthread_mutex_unlock(&l->mutex);
			return 0;
		}
		if (cond_timedwait(&l->wcond,&l->mutex,expire) == ETIMEDOUT){
			l->w_waiters--;
			pthread_mutex_unlock(&l->mutex);
			return ETIMEDOUT;
		}
	}
	pthread_mutex_unlock(&l->mutex);
	l->writer = 1;
	return 0;
}

//rwl_wunlock unlocks the lock held in the "write" mode
void
rwl_wunlock(rwl *l)
{
	pthread_mutex_lock(&l->mutex);
	l->writer = 0;
	pthread_cond_broadcast(&l->wcond);
	pthread_cond_broadcast(&l->rcond);
	pthread_mutex_unlock(&l->mutex);
}
