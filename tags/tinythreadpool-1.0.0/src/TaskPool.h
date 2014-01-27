/*
 *  Project   : TinyThreadPool
 *  File      : TaskPool.h
 *  Author    : Your Name
 *  Copyright : GPLv2
 */

#ifndef TASKPOOL_H_
#define TASKPOOL_H_
#include <vector>
#include <queue>
#include <list>
#include "Task.h"
#include "Mutex.h"
#include "Thread.h"
#include "TimeUnit.h"
#include "Timer.h"

namespace TTP
{

class TaskPool
{
    friend class ThreadPool;
public:
	TaskPool();
	~TaskPool();
	void start();
	void addTask(Task &task);
	void addTask(Task *task);
	void addPTask(Task &task);
	void addPTask(Task *task);
	Task* getTask();
	Task* getPTask();
	bool tasksPending();
	bool tasksPPending();
	static void* run(void *arg);
private:
    std::queue<Task*> *m_tasks;
    std::list<Task*> *m_ptasks;
    std::vector<Task*> *m_scheduledtasks;
    std::vector<Timer*> *m_scheduledTimers;
    Mutex *m_mutex;
    Thread *m_thread;
    volatile bool m_runFlag, m_complete, m_thrdStarted;
};

} // namespace TTP
#endif /* TASKPOOL_H_ */
