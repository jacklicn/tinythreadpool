/*
 *  Project   : TinyThreadPool
 *  File      : Thread.cc
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
#include "Thread.h"

namespace TTP
{

void* Thread::_service(void* arg)
{
    ThreadFunctor* threadFunctorPtr = static_cast<ThreadFunctor*>(arg);
    assert(threadFunctorPtr != NULL);
    void *ret = NULL;
    if(threadFunctorPtr->type == ThreadFunctor::THREAD_TYPE_FUNC) {
        ret = threadFunctorPtr->f(threadFunctorPtr->arg);
        pthread_exit(NULL);
    }
    else {
        Thread *threadPtr = static_cast<Thread*>(threadFunctorPtr->arg);
        assert(threadPtr != NULL);
        threadPtr->run();
        threadPtr->resetRunning();
    }
    return ret;
}

int Thread::yield()
{
#if defined(_POSIX_PRIORITY_SCHEDULING)
    return sched_yield();
#else
    struct timespec interval;
    interval.tv_sec  = 0;
    interval.tv_nsec = 0;
    return nanosleep(&interval,0);
#endif
}

Thread::Thread()
:m_id(-1),m_name("Thread"),m_running(false)
{
    m_threadFunctor = new ThreadFunctor();
    m_threadFunctor->f = NULL;
    m_threadFunctor->arg = this;
    m_threadFunctor->type = ThreadFunctor::THREAD_TYPE_RUN;
    pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init(&m_cond, NULL);
}

Thread::Thread(ThreadFunc f, void* arg)
:m_id(-1),m_name("Thread"),m_running(false)
{
    m_threadFunctor = new ThreadFunctor();
    m_threadFunctor->f = f;
    m_threadFunctor->arg = arg;
    m_threadFunctor->type = ThreadFunctor::THREAD_TYPE_FUNC;
    pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init(&m_cond, NULL);
}

Thread::~Thread()
{
    // request cancellation of the thread if running
    if (m_running) {
        cancel();
    }
    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_cond);
}

void Thread::join()
{
    if (m_running) {
        int status;

        // wait for thread to finish
        if ((status = pthread_join( m_pthread, NULL )) != 0)
        {
            std::cerr << "Thread join : pthread_join ("
                    << strerror( status ) << ")" << std::endl;
        }

        m_running = false;
    }// if
}

void Thread::nSleep(long nanos)
{
    struct timespec req={0},rem={0};
    req.tv_sec = 0;
    req.tv_nsec = nanos;
    int ret = nanosleep(&req, &rem);
    if(ret==-1) {
        struct timespec temp_rem;
        ret = nanosleep(&req, &temp_rem);
    }
}

void Thread::uSleep(long micros)
{
    usleep(micros);
}

void Thread::mSleep(long milis)
{
    usleep(milis*1000);
}

void Thread::sSleep(long seconds)
{
    sleep(seconds);
}

void Thread::wait()
{
    if(m_running) {
        pthread_mutex_lock(&m_mutex);
        pthread_cond_wait(&m_cond, &m_mutex);
        pthread_mutex_unlock(&m_mutex);
    }
}

void Thread::execute(const bool detached,const bool sscope)
{
    if (!m_running)
    {
        int status;
        pthread_attr_t  thread_attr;

        if ((status = pthread_attr_init(&thread_attr)) != 0) {
            std::cerr << "Thread create : pthread_attr_init ("
                    << strerror( status ) << ")" << std::endl;
            return;
        }// if

        if (detached) {
            // detache created thread from calling thread
            if ((status = pthread_attr_setdetachstate(&thread_attr,
                    PTHREAD_CREATE_DETACHED)) != 0) {
                std::cerr << "Thread create : pthread_attr_setdetachstate ("
                        << strerror( status ) << ")" << std::endl;
                return;
            }// if
        }// if

        if (sscope) {
            // use system-wide scheduling for thread
            if ((status = pthread_attr_setscope(&thread_attr,PTHREAD_SCOPE_SYSTEM)) != 0) {
                std::cerr << "Thread create : pthread_attr_setscope ("
                        << strerror( status ) << ")" << std::endl;
                return;
            }// if
        }// if

#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING) && defined(SUNOS)
        //
        // adjust thread-scheduling for Solaris
        //

        struct sched_param  t_param;

        t_param.sched_priority = sched_get_priority_min(SCHED_RR);

        if ((status = pthread_attr_setschedpolicy(&thread_attr,SCHED_RR)) != 0) {
            std::cerr << "Thread create : pthread_attr_setschedpolicy ("
                    << strerror( status ) << ")" << std::endl;
        }

        if ((status = pthread_attr_setschedparam(&thread_attr,&t_param)) != 0) {
            std::cerr << "Thread create : pthread_attr_setschedparam ("
            << strerror( status ) << ")" << std::endl;
        }

        if ((status = pthread_attr_setinheritsched(&thread_attr,PTHREAD_EXPLICIT_SCHED)) != 0) {
            std::cerr << "Thread create : pthread_attr_setinheritsched ("
            << strerror( status ) << ")" << std::endl;
        }
#endif

#ifdef HPUX
        // on HP-UX we increase the stack-size for a stable behaviour
        // (need much memory for this !!!)
        pthread_attr_setstacksize(&thread_attr,2*1024*1024);
#endif

        m_running = true;

        if ((status = pthread_create(&m_pthread, &thread_attr,_service,m_threadFunctor)) != 0) {
            std::cerr << "Thread create : pthread_create ("
                    << strerror(status) << ")" << std::endl;
        }
        else {
            m_running = false; // THREAD_TYPE_FUNC
        }

        // remove attribute
        pthread_attr_destroy(&thread_attr);

    }// if
    else
    {
        std::cout << "Thread create : thread is already running" << std::endl;
    }
}

bool Thread::isRunning() const
{
    return m_running;
}

void Thread::interrupt()
{
    if(m_running) {
        pthread_mutex_lock(&m_mutex);
        pthread_cond_broadcast(&m_cond);
        pthread_mutex_unlock(&m_mutex);
    }
}

void Thread::setId (const int id)
{
    m_id = id;
}

int Thread::getId() const
{
    return m_id;
}
void Thread::setName(const char *name)
{
    if(name != NULL) {
        m_name = name;
    }
}
void Thread::setName(const std::string &name)
{
    m_name = name;
}

std::string Thread::getName() const
{
    return m_name;
}

void Thread::resetRunning()
{
    m_running = false;
}

void Thread::detach ()
{
    if (m_running) {
        int status;
        // detach thread
        if ((status = pthread_detach(m_pthread)) != 0) {
            std::cerr << "Thread detach : pthread_detach ("
                    << strerror(status) << ")" << std::endl;
        }
    }// if
}

void Thread::cancel()
{
    if (m_running) {
        int status;
        if ((status = pthread_cancel(m_pthread)) != 0) {
            std::cerr << "Thread cancel : pthread_cancel ("
                    << strerror(status) << ")" << std::endl;
        }
        m_running = false;
    }// if
}

void Thread::exit()
{
    if ( m_running && (pthread_self() == m_pthread)) {
        void *ret_val = NULL;
        pthread_exit(ret_val);
        m_running = false;
    }// if
}

} // namespace TTP
