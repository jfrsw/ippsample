/*
 * Threading primitives for CUPS.
 *
 * Copyright © 2009-2018 by Apple Inc.
 *
 * Licensed under Apache License v2.0.  See the file "LICENSE" for more
 * information.
 */

/*
 * Include necessary headers...
 */

#include "cups-private.h"
#include "thread-private.h"


#if defined(HAVE_PTHREAD_H)

#	if defined(_WIN32)
/* This part taken from Linux time.h */
 
 /*
  * The IDs of the various system clocks (for POSIX.1b interval timers):
  */
#define CLOCK_REALTIME			0
#define CLOCK_MONOTONIC			1
#define CLOCK_PROCESS_CPUTIME_ID	2
#define CLOCK_THREAD_CPUTIME_ID		3
#define CLOCK_MONOTONIC_RAW		4
#define CLOCK_REALTIME_COARSE		5
#define CLOCK_MONOTONIC_COARSE		6
#define CLOCK_BOOTTIME			7
#define CLOCK_REALTIME_ALARM		8
#define CLOCK_BOOTTIME_ALARM		9
  /*
   * The driver implementing this got removed. The clock ID is kept as a
   * place holder. Do not reuse!
   */
#define CLOCK_SGI_CYCLE			10
#define CLOCK_TAI			11

int clock_gettime(int clockId, struct timespec *spec)      //C-file part
{
	__int64 wintime; GetSystemTimeAsFileTime((FILETIME*)&wintime);
	wintime -= 116444736000000000i64;  //1jan1601 to 1jan1970
	spec->tv_sec = wintime / 10000000i64;           //seconds
	spec->tv_nsec = wintime % 10000000i64 * 100;      //nano-seconds
	return 0;
}
#	endif _WIN32


/*
 * '_cupsCondBroadcast()' - Wake up waiting threads.
 */

void
_cupsCondBroadcast(_cups_cond_t *cond)	/* I - Condition */
{
  pthread_cond_broadcast(cond);
}


/*
 * '_cupsCondInit()' - Initialize a condition variable.
 */

void
_cupsCondInit(_cups_cond_t *cond)	/* I - Condition */
{
  pthread_cond_init(cond, NULL);
}


/*
 * '_cupsCondWait()' - Wait for a condition with optional timeout.
 */

void
_cupsCondWait(_cups_cond_t  *cond,	/* I - Condition */
              _cups_mutex_t *mutex,	/* I - Mutex */
	      double        timeout)	/* I - Timeout in seconds (0 or negative for none) */
{
  if (timeout > 0.0)
  {
    struct timespec abstime;		/* Timeout */

    clock_gettime(CLOCK_REALTIME, &abstime);

    abstime.tv_sec  += (long)timeout;
    abstime.tv_nsec += (long)(1000000000 * (timeout - (long)timeout));

    while (abstime.tv_nsec >= 1000000000)
    {
      abstime.tv_nsec -= 1000000000;
      abstime.tv_sec ++;
    };

    pthread_cond_timedwait(cond, mutex, &abstime);
  }
  else
    pthread_cond_wait(cond, mutex);
}


/*
 * '_cupsMutexInit()' - Initialize a mutex.
 */

void
_cupsMutexInit(_cups_mutex_t *mutex)	/* I - Mutex */
{
  pthread_mutex_init(mutex, NULL);
}


/*
 * '_cupsMutexLock()' - Lock a mutex.
 */

void
_cupsMutexLock(_cups_mutex_t *mutex)	/* I - Mutex */
{
  pthread_mutex_lock(mutex);
}


/*
 * '_cupsMutexUnlock()' - Unlock a mutex.
 */

void
_cupsMutexUnlock(_cups_mutex_t *mutex)	/* I - Mutex */
{
  pthread_mutex_unlock(mutex);
}


/*
 * '_cupsRWInit()' - Initialize a reader/writer lock.
 */

void
_cupsRWInit(_cups_rwlock_t *rwlock)	/* I - Reader/writer lock */
{
  pthread_rwlock_init(rwlock, NULL);
}


/*
 * '_cupsRWLockRead()' - Acquire a reader/writer lock for reading.
 */

void
_cupsRWLockRead(_cups_rwlock_t *rwlock)	/* I - Reader/writer lock */
{
  pthread_rwlock_rdlock(rwlock);
}


/*
 * '_cupsRWLockWrite()' - Acquire a reader/writer lock for writing.
 */

void
_cupsRWLockWrite(_cups_rwlock_t *rwlock)/* I - Reader/writer lock */
{
  pthread_rwlock_wrlock(rwlock);
}


/*
 * '_cupsRWUnlock()' - Release a reader/writer lock.
 */

void
_cupsRWUnlock(_cups_rwlock_t *rwlock)	/* I - Reader/writer lock */
{
  pthread_rwlock_unlock(rwlock);
}


/*
 * '_cupsThreadCancel()' - Cancel (kill) a thread.
 */

void
_cupsThreadCancel(_cups_thread_t thread)/* I - Thread ID */
{
  pthread_cancel(thread);
}


/*
 * '_cupsThreadCreate()' - Create a thread.
 */

_cups_thread_t				/* O - Thread ID */
_cupsThreadCreate(
    _cups_thread_func_t func,		/* I - Entry point */
    void                *arg)		/* I - Entry point context */
{
  pthread_t thread;

  if (pthread_create(&thread, NULL, (void *(*)(void *))func, arg))
#if defined(PTW32_VERSION)
  {
		thread.p = NULL;
		thread.x = 0;
		return(thread);
  }
#else
		return (0);
#endif
  else
    return (thread);
}


/*
 * '_cupsThreadDetach()' - Tell the OS that the thread is running independently.
 */

void
_cupsThreadDetach(_cups_thread_t thread)/* I - Thread ID */
{
  pthread_detach(thread);
}


/*
 * '_cupsThreadWait()' - Wait for a thread to exit.
 */

void *					/* O - Return value */
_cupsThreadWait(_cups_thread_t thread)	/* I - Thread ID */
{
  void	*ret;				/* Return value */


  if (pthread_join(thread, &ret))
    return (NULL);
  else
    return (ret);
}


#elif defined(_WIN32)
#  include <process.h>


/*
 * '_cupsCondBroadcast()' - Wake up waiting threads.
 */

void
_cupsCondBroadcast(_cups_cond_t *cond)	/* I - Condition */
{
  // TODO: Implement me
}


/*
 * '_cupsCondInit()' - Initialize a condition variable.
 */

void
_cupsCondInit(_cups_cond_t *cond)	/* I - Condition */
{
  // TODO: Implement me
}


/*
 * '_cupsCondWait()' - Wait for a condition with optional timeout.
 */

void
_cupsCondWait(_cups_cond_t  *cond,	/* I - Condition */
              _cups_mutex_t *mutex,	/* I - Mutex */
	      double        timeout)	/* I - Timeout in seconds (0 or negative for none) */
{
  // TODO: Implement me
}


/*
 * '_cupsMutexInit()' - Initialize a mutex.
 */

void
_cupsMutexInit(_cups_mutex_t *mutex)	/* I - Mutex */
{
  InitializeCriticalSection(&mutex->m_criticalSection);
  mutex->m_init = 1;
}


/*
 * '_cupsMutexLock()' - Lock a mutex.
 */

void
_cupsMutexLock(_cups_mutex_t *mutex)	/* I - Mutex */
{
  if (!mutex->m_init)
  {
    _cupsGlobalLock();

    if (!mutex->m_init)
    {
      InitializeCriticalSection(&mutex->m_criticalSection);
      mutex->m_init = 1;
    }

    _cupsGlobalUnlock();
  }

  EnterCriticalSection(&mutex->m_criticalSection);
}


/*
 * '_cupsMutexUnlock()' - Unlock a mutex.
 */

void
_cupsMutexUnlock(_cups_mutex_t *mutex)	/* I - Mutex */
{
  LeaveCriticalSection(&mutex->m_criticalSection);
}


/*
 * '_cupsRWInit()' - Initialize a reader/writer lock.
 */

void
_cupsRWInit(_cups_rwlock_t *rwlock)	/* I - Reader/writer lock */
{
  _cupsMutexInit((_cups_mutex_t *)rwlock);
}


/*
 * '_cupsRWLockRead()' - Acquire a reader/writer lock for reading.
 */

void
_cupsRWLockRead(_cups_rwlock_t *rwlock)	/* I - Reader/writer lock */
{
  _cupsMutexLock((_cups_mutex_t *)rwlock);
}


/*
 * '_cupsRWLockWrite()' - Acquire a reader/writer lock for writing.
 */

void
_cupsRWLockWrite(_cups_rwlock_t *rwlock)/* I - Reader/writer lock */
{
  _cupsMutexLock((_cups_mutex_t *)rwlock);
}


/*
 * '_cupsRWUnlock()' - Release a reader/writer lock.
 */

void
_cupsRWUnlock(_cups_rwlock_t *rwlock)	/* I - Reader/writer lock */
{
  _cupsMutexUnlock((_cups_mutex_t *)rwlock);
}


/*
 * '_cupsThreadCancel()' - Cancel (kill) a thread.
 */

void
_cupsThreadCancel(_cups_thread_t thread)/* I - Thread ID */
{
  // TODO: Implement me
}


/*
 * '_cupsThreadCreate()' - Create a thread.
 */

_cups_thread_t				/* O - Thread ID */
_cupsThreadCreate(
    _cups_thread_func_t func,		/* I - Entry point */
    void                *arg)		/* I - Entry point context */
{
  return (_beginthreadex(NULL, 0, (LPTHREAD_START_ROUTINE)func, arg, 0, NULL));
}


/*
 * '_cupsThreadDetach()' - Tell the OS that the thread is running independently.
 */

void
_cupsThreadDetach(_cups_thread_t thread)/* I - Thread ID */
{
  // TODO: Implement me
  (void)thread;
}


/*
 * '_cupsThreadWait()' - Wait for a thread to exit.
 */

void *					/* O - Return value */
_cupsThreadWait(_cups_thread_t thread)	/* I - Thread ID */
{
  // TODO: Implement me
  (void)thread;

  return (NULL);
}


#else /* No threading */
/*
 * '_cupsCondBroadcast()' - Wake up waiting threads.
 */

void
_cupsCondBroadcast(_cups_cond_t *cond)	/* I - Condition */
{
  // TODO: Implement me
}


/*
 * '_cupsCondInit()' - Initialize a condition variable.
 */

void
_cupsCondInit(_cups_cond_t *cond)	/* I - Condition */
{
  // TODO: Implement me
}


/*
 * '_cupsCondWait()' - Wait for a condition with optional timeout.
 */

void
_cupsCondWait(_cups_cond_t  *cond,	/* I - Condition */
              _cups_mutex_t *mutex,	/* I - Mutex */
	      double        timeout)	/* I - Timeout in seconds (0 or negative for none) */
{
  // TODO: Implement me
}


/*
 * '_cupsMutexInit()' - Initialize a mutex.
 */

void
_cupsMutexInit(_cups_mutex_t *mutex)	/* I - Mutex */
{
  (void)mutex;
}


/*
 * '_cupsMutexLock()' - Lock a mutex.
 */

void
_cupsMutexLock(_cups_mutex_t *mutex)	/* I - Mutex */
{
  (void)mutex;
}


/*
 * '_cupsMutexUnlock()' - Unlock a mutex.
 */

void
_cupsMutexUnlock(_cups_mutex_t *mutex)	/* I - Mutex */
{
  (void)mutex;
}


/*
 * '_cupsRWInit()' - Initialize a reader/writer lock.
 */

void
_cupsRWInit(_cups_rwlock_t *rwlock)	/* I - Reader/writer lock */
{
  (void)rwlock;
}


/*
 * '_cupsRWLockRead()' - Acquire a reader/writer lock for reading.
 */

void
_cupsRWLockRead(_cups_rwlock_t *rwlock)	/* I - Reader/writer lock */
{
  (void)rwlock;
}


/*
 * '_cupsRWLockWrite()' - Acquire a reader/writer lock for writing.
 */

void
_cupsRWLockWrite(_cups_rwlock_t *rwlock)/* I - Reader/writer lock */
{
  (void)rwlock;
}


/*
 * '_cupsRWUnlock()' - Release a reader/writer lock.
 */

void
_cupsRWUnlock(_cups_rwlock_t *rwlock)	/* I - Reader/writer lock */
{
  (void)rwlock;
}


/*
 * '_cupsThreadCancel()' - Cancel (kill) a thread.
 */

void
_cupsThreadCancel(_cups_thread_t thread)/* I - Thread ID */
{
  (void)thread;
}


/*
 * '_cupsThreadCreate()' - Create a thread.
 */

_cups_thread_t				/* O - Thread ID */
_cupsThreadCreate(
    _cups_thread_func_t func,		/* I - Entry point */
    void                *arg)		/* I - Entry point context */
{
  fputs("DEBUG: CUPS was compiled without threading support, no thread created.\n", stderr);

  (void)func;
  (void)arg;

  return (0);
}


/*
 * '_cupsThreadDetach()' - Tell the OS that the thread is running independently.
 */

void
_cupsThreadDetach(_cups_thread_t thread)/* I - Thread ID */
{
  (void)thread;
}


/*
 * '_cupsThreadWait()' - Wait for a thread to exit.
 */

void *					/* O - Return value */
_cupsThreadWait(_cups_thread_t thread)	/* I - Thread ID */
{
  (void)thread;

  return (NULL);
}

#endif /* HAVE_PTHREAD_H */
