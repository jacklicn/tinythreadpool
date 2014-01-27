/*
 *  Project   : TinyThreadPool
 *  File      : Mutex.cc
 *  Author    : Your Name
 *  Copyright : GPLv2
 */

#include <unistd.h>
#include <signal.h>
#include <time.h> // nanosleep
#include <sys/time.h> // gettimeofday
#include <sched.h>
#include <string.h>
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include "Mutex.h"

namespace TTP
{

//
// Semaphore
//
Semaphore::Semaphore(int n)
{
    Semaphore(n,n);
}

Semaphore::Semaphore(int n, int max)
{
    assert(n >= 0 && max > 0 && n <= max);

    _n = n;
    _max = max;

    if (pthread_mutex_init(&_mutex, NULL)) {
        fprintf(stderr,"cannot create semaphore (mutex)");
        throw;
    }
    if (pthread_cond_init(&_cond, NULL)) {
        fprintf(stderr,"cannot create semaphore (condition)");
        throw;
    }
}

Semaphore::~Semaphore()
{
    pthread_cond_destroy(&_cond);
    pthread_mutex_destroy(&_mutex);
}

void Semaphore::set()
{
    if (pthread_mutex_lock(&_mutex)) {
        fprintf(stderr,"cannot signal semaphore (lock)");
        throw;
    }

    if (_n < _max) {
        ++_n;
    }
    else {
        pthread_mutex_unlock(&_mutex);
        fprintf(stderr,"cannot signal semaphore: count would exceed maximum");
        throw;
    }

    if (pthread_cond_signal(&_cond)) {
        pthread_mutex_unlock(&_mutex);
        fprintf(stderr,"cannot signal semaphore");
        throw;
    }

    pthread_mutex_unlock(&_mutex);
}

void Semaphore::wait()
{
    if (pthread_mutex_lock(&_mutex)) {
        fprintf(stderr,"wait for semaphore failed (lock)");
        throw;
    }

    while (_n < 1) {
        if (pthread_cond_wait(&_cond, &_mutex)) {
            pthread_mutex_unlock(&_mutex);
            fprintf(stderr,"wait for semaphore failed");
            throw;
        }
    }

    --_n;

    pthread_mutex_unlock(&_mutex);
}

bool Semaphore::tryWait(long milliseconds)
{
    int rc = 0;
    struct timespec abstime;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    abstime.tv_sec  = tv.tv_sec + milliseconds / 1000;
    abstime.tv_nsec = tv.tv_usec*1000 + (milliseconds % 1000) * 1000000;
    if (abstime.tv_nsec >= 1000000000) {
        abstime.tv_nsec -= 1000000000;
        ++abstime.tv_sec;
    }

    if (pthread_mutex_lock(&_mutex) != 0) {
        fprintf(stderr,"wait for semaphore failed (lock)");
        throw;
    }

    while (_n < 1) {
        if ((rc = pthread_cond_timedwait(&_cond, &_mutex, &abstime))) {
            if (rc == ETIMEDOUT) break;
            pthread_mutex_unlock(&_mutex);
            fprintf(stderr,"cannot wait for semaphore");
            throw;
        }
    }

    if (rc == 0) {
        --_n;
    }

    pthread_mutex_unlock(&_mutex);

    return rc == 0;
}

//
// Mutex
//
Mutex::Mutex()
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);

#if defined(PTHREAD_MUTEX_RECURSIVE_NP)
    pthread_mutexattr_settype_np(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
#endif

    if (pthread_mutex_init(&_mutex, &attr)) {
        pthread_mutexattr_destroy(&attr);
        fprintf(stderr,"cannot create mutex");
        throw;
    }
    pthread_mutexattr_destroy(&attr);
}

Mutex::Mutex(bool fast)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);

#if defined(PTHREAD_MUTEX_RECURSIVE_NP)
    pthread_mutexattr_settype_np(&attr, fast ? PTHREAD_MUTEX_NORMAL_NP : PTHREAD_MUTEX_RECURSIVE_NP);
#endif

    if (pthread_mutex_init(&_mutex, &attr)) {
        pthread_mutexattr_destroy(&attr);
        fprintf(stderr,"cannot create mutex");
        throw;
    }
    pthread_mutexattr_destroy(&attr);
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&_mutex);
}

bool Mutex::tryLock()
{
    int rc = pthread_mutex_trylock(&_mutex);
    if (rc == 0) {
        return true;
    }
    else if (rc == EBUSY) {
        return false;
    }
    else {
        fprintf(stderr,"cannot lock mutex");
        return false; // friendly
    }
}

bool Mutex::tryLock(long milliseconds)
{
    struct timespec abstime;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    abstime.tv_sec  = tv.tv_sec + milliseconds / 1000;
    abstime.tv_nsec = tv.tv_usec*1000 + (milliseconds % 1000) * 1000000;
    if (abstime.tv_nsec >= 1000000000) {
        abstime.tv_nsec -= 1000000000;
        ++abstime.tv_sec;
    }
    int rc = pthread_mutex_timedlock(&_mutex, &abstime);
    if (rc == 0) {
        return true;
    }
    else if (rc == ETIMEDOUT) {
        return false;
    }
    else {
        fprintf(stderr,"cannot lock mutex");
        return false; // friendly
    }

}

//
// RWLock
//
RWLock::RWLock()
{
    if (pthread_rwlock_init(&_rwl, NULL)) {
        fprintf(stderr,"cannot create reader/writer lock");
        throw;
    }
}

RWLock::~RWLock()
{
    pthread_rwlock_destroy(&_rwl);
}

void RWLock::readLock()
{
    if (pthread_rwlock_rdlock(&_rwl)) {
        fprintf(stderr,"cannot lock reader/writer lock");
        throw;
    }
}

bool RWLock::tryReadLock()
{
    int rc = pthread_rwlock_tryrdlock(&_rwl);
    if (rc == 0) {
        return true;
    }
    else if (rc == EBUSY) {
        return false;
    }
    else {
        fprintf(stderr,"cannot lock reader/writer lock");
        return false; // friendly
    }

}

void RWLock::writeLock()
{
    if (pthread_rwlock_wrlock(&_rwl)) {
        fprintf(stderr,"cannot lock reader/writer lock");
        throw;
    }
}

bool RWLock::tryWriteLock()
{
    int rc = pthread_rwlock_trywrlock(&_rwl);
    if (rc == 0) {
        return true;
    }
    else if (rc == EBUSY) {
        return false;
    }
    else {
        fprintf(stderr,"cannot lock reader/writer lock");
        return false; // friendly
    }
}

void RWLock::unlock()
{
    if (pthread_rwlock_unlock(&_rwl)) {
        fprintf(stderr,"cannot unlock mutex");
        throw;
    }
}

//
// ScopedRWLock
//
ScopedRWLock::ScopedRWLock(RWLock& rwl, bool write)
: _rwl(rwl)
{
    if (write) {
        _rwl.writeLock();
    }
    else {
        _rwl.readLock();
    }
}

ScopedRWLock::~ScopedRWLock()
{
    _rwl.unlock();
}

//
// ScopedReadRWLock
//
ScopedReadRWLock::ScopedReadRWLock(RWLock& rwl)
: ScopedRWLock(rwl, false)
{
}

ScopedReadRWLock::~ScopedReadRWLock()
{
}

//
// ScopedWriteRWLock
//
ScopedWriteRWLock::ScopedWriteRWLock(RWLock& rwl)
: ScopedRWLock(rwl, true)
{
}

ScopedWriteRWLock::~ScopedWriteRWLock()
{
}

} // namespace TTP

