/*
 *  Project   : TinyThreadPool
 *  File      : Mutex.h
 *  Author    : Your Name
 *  Copyright : GPLv2
 */

#ifndef MUTEX_H_
#define MUTEX_H_
#include <cstdio>
#include <pthread.h>

namespace TTP
{

// A Semaphore is a synchronization object with the following
// characteristics:
// A semaphore has a value that is constrained to be a non-negative
// integer and two atomic operations. The allowable operations are V
// (here called set()) and P (here called wait()). A V (set()) operation
// increases the value of the semaphore by one.
// A P (wait()) operation decreases the value of the semaphore by one,
// provided that can be done without violating the constraint that the
// value be non-negative. A P (wait()) operation that is initiated when
// the value of the semaphore is 0 suspends the calling thread.
// The calling thread may continue when the value becomes positive again.
class Semaphore
{
public:
    // Creates the semaphore. The current value
    // of the semaphore is given in n.
    explicit Semaphore(int n);

    // Creates the semaphore. The current value
    // of the semaphore is given in n. The
    // maximum value of the semaphore is given
    // in max.
    // If only n is given, it must be greater than
    // zero.
    // If both n and max are given, max must be
    // greater than zero, n must be greater than
    // or equal to zero and less than or equal
    // to max.
    Semaphore(int n, int max);

    // Destroys the semaphore.
    ~Semaphore();

    // Increments the semaphore's value by one and
    // thus signals the semaphore. Another thread
    // waiting for the semaphore will be able
    // to continue.
    void set();

    // Waits for the semaphore to become signalled.
    // To become signalled, a semaphore's value must
    // be greater than zero.
    // Decrements the semaphore's value by one.
    void wait();

    // Waits for the semaphore to become signalled.
    // To become signalled, a semaphore's value must
    // be greater than zero.
    // Returns true if the semaphore
    // became signalled within the specified
    // time interval, false otherwise.
    // Decrements the semaphore's value by one
    // if successful.
    bool tryWait(long milliseconds);

private:
    Semaphore();
    Semaphore(const Semaphore&);
    Semaphore& operator = (const Semaphore&);

private:
    volatile int    _n;
    int             _max;
    pthread_mutex_t _mutex;
    pthread_cond_t  _cond;
};

// A Mutex (mutual exclusion) is a synchronization
// mechanism used to control access to a shared resource
// in a concurrent (multithreaded) scenario.
// Mutexes are recursive, that is, the same mutex can be
// locked multiple times by the same thread (but, of course,
// not by other threads).
class Mutex
{
public:
    // creates the Mutex.
    Mutex();

    // if fast is true,Mutex is normal,
    // otherwise Mutex is recursive.
    explicit Mutex(bool fast);

    // destroys the Mutex.
    ~Mutex();

    // Locks the mutex. Blocks if the mutex
    // is held by another thread.
    void lock();

    // Tries to lock the mutex. Returns false immediately
    // if the mutex is already held by another thread.
    // Returns true if the mutex was successfully locked.
    bool tryLock();

    // Locks the mutex. Blocks up to the given number of milliseconds
    // if the mutex is held by another thread.
    // Returns true if the mutex was successfully locked.
    bool tryLock(long milliseconds);

    // Unlocks the mutex so that it can be acquired by
    // other threads.
    void unlock();

    // return true if mutex is locked and false, otherwise
    bool is_locked ();

private:
    Mutex(const Mutex&);
    Mutex& operator = (const Mutex&);

protected:
    pthread_mutex_t _mutex;
};



// A class that simplifies thread synchronization
// with a mutex.
// The constructor accepts a Mutex and locks it.
// The destructor unlocks the mutex.
class ScopedLock
{
public:
    ScopedLock(Mutex& mutex);
    ~ScopedLock();

private:
    ScopedLock();
    ScopedLock(const ScopedLock&);
    ScopedLock& operator = (const ScopedLock&);

private:
    Mutex& _mutex;
};



// class for a condition variable
// derived from mutex to allow locking of condition
// to inspect or modify the predicate
class Condition : public Mutex
{
public:
    // constructor
    Condition();

    // destructor
    ~Condition();

    // wait for signal to arrive
    void wait();

    // restart one of the threads, waiting on the cond. variable
    void signal();

    // restart all waiting threads
    void broadcast();

private:
    // our condition variable
    pthread_cond_t  _cond;
};



// A reader writer lock allows multiple concurrent
// readers or one exclusive writer.
class RWLock
{
public:
    // Creates the Reader/Writer lock.
    RWLock();

    // Destroys the Reader/Writer lock.
    ~RWLock();

    // Acquires a read lock. If another thread currently holds a write lock,
    // waits until the write lock is released.
    void readLock();

    // Tries to acquire a read lock. Immediately returns true if successful, or
    // false if another thread currently holds a write lock.
    bool tryReadLock();

    // Acquires a write lock. If one or more other threads currently hold
    // locks, waits until all locks are released. The results are undefined
    // if the same thread already holds a read or write lock
    void writeLock();

    // Tries to acquire a write lock. Immediately returns true if successful,
    // or false if one or more other threads currently hold
    // locks. The result is undefined if the same thread already
    // holds a read or write lock.
    bool tryWriteLock();

    // Releases the read or write lock.
    void unlock();

private:
    RWLock(const RWLock&);
    RWLock& operator = (const RWLock&);

private:
    pthread_rwlock_t _rwl;
};

// A variant of ScopedLock for reader/writer locks.
class ScopedRWLock
{
public:
    ScopedRWLock(RWLock& rwl, bool write = false);
    ~ScopedRWLock();

private:
    ScopedRWLock();
    ScopedRWLock(const ScopedRWLock&);
    ScopedRWLock& operator = (const ScopedRWLock&);

private:
    RWLock& _rwl;
};

// A variant of ScopedLock for reader locks.
class ScopedReadRWLock : public ScopedRWLock
{
public:
    ScopedReadRWLock(RWLock& rwl);
    ~ScopedReadRWLock();
};

// A variant of ScopedLock for writer locks.
class ScopedWriteRWLock : public ScopedRWLock
{
public:
    ScopedWriteRWLock(RWLock& rwl);
    ~ScopedWriteRWLock();
};

// An Event is a synchronization object that
// allows one thread to signal one or more
// other threads that a certain event
// has happened.
// Usually, one thread signals an event,
// while one or more other threads wait
// for an event to become signalled.
class Event
{
public:
    // Creates the event. If autoReset is true,
    // the event is automatically reset after
    // a wait() successfully returns.
    Event(bool autoReset = true);

    // Destroys the event.
    ~Event();

    // Signals the event. If autoReset is true,
    // only one thread waiting for the event
    // can resume execution.
    // If autoReset is false, all waiting threads
    // can resume execution.
    void set();

    // Waits for the event to become signalled.
    void wait();

    // Waits for the event to become signalled.
    // return false if the event
    // does not become signalled within the specified
    // time interval, false otherwise.
    bool wait(long milliseconds);

    // Resets the event to unsignalled state.
    void reset();

private:
    Event(const Event&);
    Event& operator = (const Event&);

private:
    bool            _auto;
    volatile bool   _state;
    pthread_mutex_t _mutex;
    pthread_cond_t  _cond;
};

} // namespace TTP
#endif /* MUTEX_H_ */
