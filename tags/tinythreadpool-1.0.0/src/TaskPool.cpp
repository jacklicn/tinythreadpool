/*
 *  Project   : TinyThreadPool
 *  File      : TaskPool.cpp
 *  Author    : Your Name
 *  Copyright : GPLv2
 */

#include <assert.h>
#include "TaskPool.h"

namespace TTP
{

void* TaskPool::run(void *arg)
{
	TaskPool* pool = static_cast<TaskPool*>(arg);
	assert(pool != NULL);
	std::vector<Task*>::iterator iter;
	pool->m_mutex->lock();
	bool fl = pool->m_runFlag;
	pool->m_mutex->unlock();
	while(fl)
	{
		pool->m_mutex->lock();
		int total = pool->m_scheduledtasks->size();
		pool->m_mutex->unlock();
		std::queue<int> tobeRemoved;
		for (int i=0; i<total;i++)
		{
			pool->m_mutex->lock();
			Task* task = pool->m_scheduledtasks->at(i);
			Timer* timer = pool->m_scheduledTimers->at(i);
			pool->m_mutex->unlock();
			if(task!=NULL)
			{
				if(task->isWaitOver(timer))
				{
					tobeRemoved.push(i);
					pool->m_mutex->lock();
					pool->m_tasks->push(task);
					pool->m_mutex->unlock();
				}
			}
		}
		int counter = 0;
		while (!tobeRemoved.empty())
		{
			int index = tobeRemoved.front() - counter;
			counter++;
			tobeRemoved.pop();
			pool->m_mutex->lock();
			Timer* timer = pool->m_scheduledTimers->at(index);
			pool->m_scheduledtasks->erase(pool->m_scheduledtasks->begin()+index);
			pool->m_scheduledTimers->erase(pool->m_scheduledTimers->begin()+index);
			delete timer;
			pool->m_mutex->unlock();
		}
		Thread::nSleep(1);
		pool->m_mutex->lock();
		fl = pool->m_runFlag;
		pool->m_mutex->unlock();
	}
	pool->m_mutex->lock();
	pool->m_complete = true;
	pool->m_mutex->unlock();
	return NULL;
}

TaskPool::TaskPool()
{
	m_mutex = new Mutex();
	m_tasks = new std::queue<Task*>;
	m_ptasks = new std::list<Task*>;
	m_scheduledtasks = new std::vector<Task*>;
	m_scheduledTimers = new std::vector<Timer*>;
	m_runFlag = true;
	m_complete = false;
	m_thread = new Thread(&run, this);
	m_thrdStarted = false;
}

void TaskPool::start()
{
	if(m_thrdStarted)
	{
	    return;
	}
	m_thread->execute();
	m_thrdStarted = true;
}

void TaskPool::addTask(Task &task)
{
	m_mutex->lock();
	if (task.m_type >= 0 && task.m_type <= 6 && task.m_tunit > 0)
	{
		Timer* t = new Timer;
		t->start();
		m_scheduledTimers->push_back(t);
		m_scheduledtasks->push_back(&task);
	}
	else
	{
	    m_tasks->push(&task);
	}
	m_mutex->unlock();
}

void TaskPool::addTask(Task *task)
{
	m_mutex->lock();
	if (task->m_type >= 0 && task->m_type <= 6 && task->m_tunit > 0)
	{
		Timer* t = new Timer;
		t->start();
		m_scheduledTimers->push_back(t);
		m_scheduledtasks->push_back(task);
	}
	else
	{
	    m_tasks->push(task);
	}
	m_mutex->unlock();
}

void TaskPool::addPTask(Task &task)
{
	m_mutex->lock();
	m_ptasks->push_back(&task);
	m_mutex->unlock();
}

void TaskPool::addPTask(Task *task)
{
	m_mutex->lock();
	m_ptasks->push_back(task);
	m_mutex->unlock();
}

Task* TaskPool::getTask()
{
	Task *task = NULL;
	m_mutex->lock();
	if(!m_tasks->empty())
	{
		task = m_tasks->front();
		m_tasks->pop();
	}
	m_mutex->unlock();
	return task;
}
Task* TaskPool::getPTask()
{
	m_mutex->lock();
	int currpri = 0;
	Task *task = NULL;
	std::list<Task*>::iterator iter, iter1;
	for (iter = m_ptasks->begin(); iter != m_ptasks->end(); ++iter)
	{
		if ((*iter)->m_priority > currpri)
		{
			task = *iter;
			iter1 = iter;
			currpri = task->m_priority;
		}
	}
	if(task != NULL)
	{
	    m_ptasks->remove(task);
	}
	m_mutex->unlock();
	return task;
}
bool TaskPool::tasksPending()
{
	m_mutex->lock();
	bool tp = !m_tasks->empty();
	tp |= !m_scheduledtasks->empty();
	m_mutex->unlock();
	return tp;
}
bool TaskPool::tasksPPending()
{
	m_mutex->lock();
	bool tp = !m_ptasks->empty();
	tp |= !m_scheduledtasks->empty();
	m_mutex->unlock();
	return tp;
}
TaskPool::~TaskPool()
{
	m_mutex->lock();
	m_runFlag = false;
	m_mutex->unlock();
	m_mutex->lock();
	bool fl = m_complete;
	m_mutex->unlock();
	while(!fl)
	{
		m_mutex->lock();
		fl = m_complete;
		m_mutex->unlock();
		Thread::mSleep(1);
	}
	delete m_thread;
	delete m_tasks;
	delete m_ptasks;
	delete m_scheduledtasks;
	delete m_scheduledTimers;
	delete m_mutex;
}

} // namespace TTP
