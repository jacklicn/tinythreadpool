/*
 *  Project   : TinyThreadPool
 *  File      : PoolThread.h
 *  Author    : Your Name
 *  Copyright : GPLv2
 */

#ifndef POOLTHREAD_H_
#define POOLTHREAD_H_
#include "Task.h"
#include "Thread.h"
#include "Mutex.h"
#include "TimeUnit.h"

namespace TTP
{

class PoolThread
{
    friend class ThreadPool;
public:
    PoolThread();
    virtual ~PoolThread();
	bool isIdle();
	void checkout(Task *task);
	void execute();
    void release();
    Task* getTask();
    static void* run(void *arg);
private:
    Thread *m_thread;
    bool m_idle;
    Task *m_task;
    Mutex *m_mutex;
    volatile bool m_runFlag, m_complete, m_thrdStarted;
};

} // namespace TTP
#endif /* POOLTHREAD_H_ */
