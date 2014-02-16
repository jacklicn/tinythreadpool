/*
 *  Project   : TinyThreadPool
 *  File      : Thread.h
 *  Author    : Your Name
 *  Copyright : GPLv2
 */

#ifndef THREAD_H_
#define THREAD_H_
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string>

namespace TTP
{

namespace
{
    typedef void* (*ThreadFunc)(void*);
} // namespace anonymous

class Thread;

class ThreadFunctor
{
	friend class Thread;
public:
    enum ThreadType
    {
        THREAD_TYPE_FUNC = 0, // ThreadFunc
        THREAD_TYPE_RUN  = 1  // run()
    };
	ThreadFunctor():thread(NULL),f(NULL),arg(NULL),type(THREAD_TYPE_FUNC){}
	~ThreadFunctor(){}
public:
	Thread *thread;
	ThreadFunc f;
	void* arg;
	int type;
};

class Thread
{
public:
    Thread(); // for run
	Thread(ThreadFunc f, void* arg); // for ThreadFunc
	virtual ~Thread();
    // execute thread
    // if detached is true, the thread will be started in detached mode,
    // e.g. can not be joined,
    // if sscope is true, the thread is started in system scope, e.g.
    // the thread competes for resources with all other threads of all
    // processes on the system; if sscope is false, the competition is
    // only process local
    void execute(const bool detached = false,const bool sscope = false);
    // actual method to be executed by thread
    virtual void run(){}
    bool isRunning() const;
	void join();
	static void nSleep(long nanos);
	static void uSleep(long micros);
	static void mSleep(long milis);
	static void sSleep(long seconds);
	void wait();
	void interrupt();
    // set thread id
    void setId (const int id);
    // return thread id
    int getId() const;
    // set thread name
    void setName(const char *name);
    void setName(const std::string &name);
    // get thread name
    std::string getName() const;
    // detach thread
    void detach();
    // request cancellation of thread
    void cancel();
    static void* _service(void* arg);
    // yield causes the calling thread to relinquish the CPU.
    // The thread is moved to the end of the queue for
    // its static priority and a new thread gets to run.
    static int yield();
public:
    //
    // internally used, but public functions
    //
    // resets running-status (used in _run_proc, see Thread.cc)
    void resetRunning();
protected:
    //
    // functions to be called by a thread itself
    //
    // terminate thread
    void exit();
private:
    // thread id
    int  m_id;
    // thread name
    std::string m_name;
    // is the thread running or not
    volatile bool m_running;
    ThreadFunctor* m_threadFunctor;
    pthread_t m_pthread;
    pthread_cond_t m_cond;
    pthread_mutex_t m_mutex;
};

} //namespace TTP

#endif /* THREAD_H_ */
