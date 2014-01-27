/*
 *  Project   : TinyThreadPool
 *  File      : ThreadPool.h
 *  Author    : Your Name
 *  Copyright : GPLv2
 */

#ifndef THREADPOOL_H_
#define THREADPOOL_H_
#include <vector>
#include "TaskPool.h"
#include "PoolThread.h"

namespace TTP
{

class ThreadPool
{
public:
	ThreadPool();
    ThreadPool(int initThreads, int maxThreads);
    ThreadPool(int initThreads, int maxThreads, int lowp, int highp);
	virtual ~ThreadPool();
	void start();
	void init(int initThreads, int maxThreads);
	void joinAll();
    void execute(Task *task, int priority);
	void execute(Task &task, int priority);
    void execute(Task *task);
	void execute(Task &task);
    void schedule(Task *task, long long tunit, int type);
	void schedule(Task &task, long long tunit, int type);
	static void* poll(void *arg);
private:
	void initializeThreads();
	void submit(Task *task);
private:
    int m_maxThreads;
    int m_initThreads;
    int m_lowp;
    int m_highp;
    std::vector<PoolThread*> *m_tpool;
    TaskPool *m_wpool;
    Thread *m_poller;
    bool m_prioritypooling;
    volatile bool m_runFlag, m_complete, m_pollerStarted;
    Mutex *m_mutex;
    bool m_joinComplete;
};

} // namespace TTP

#endif /* THREADPOOL_H_ */
