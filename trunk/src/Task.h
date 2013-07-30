/*
 *  Project   : TinyThreadPool
 *  File      : Task.h
 *  Author    : Your Name
 *  Copyright : GPLv2
 */

#ifndef TASK_H_
#define TASK_H_
#include "string"
#include "Timer.h"
#include "TimeUnit.h"

namespace TTP
{

class Task
{
	friend class PoolThread;
	friend class ThreadPool;
	friend class TaskPool;
public:
	Task();
    Task(int priority);
    Task(int tunit, int type);
	virtual ~Task();
	virtual void run() = 0;
    bool isWaitOver(Timer *timer);
public:
    int m_tunit;
    int m_type;
    int m_priority;
};

} // namespace TTP
#endif /* TASK_H_ */
