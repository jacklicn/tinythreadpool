/*
 *  Project   : TinyThreadPool
 *  File      : ThreadPool.cc
 *  Author    : Your Name
 *  Copyright : GPLv2
 */

#include <assert.h>
#include "ThreadPool.h"

namespace TTP
{

ThreadPool::ThreadPool()
{
    m_runFlag = false;
    m_joinComplete = false;
    m_wpool = NULL;
    m_maxThreads = 0;
    m_initThreads = 0;
    m_lowp = -1;
    m_highp = -1;
    m_tpool = NULL;
    m_wpool = NULL;
    m_poller = NULL;
    m_prioritypooling = false;
    m_runFlag = false;
    m_complete = false;
    m_pollerStarted = false;
    m_mutex = NULL;
    m_joinComplete = false;
}

void ThreadPool::init(int initThreads, int maxThreads)
{
	if (m_wpool != NULL) {
	    return;
	}

	m_lowp = -1;
	m_highp = -1;
	m_initThreads = initThreads;
	m_maxThreads = maxThreads;
	m_joinComplete = false;
	m_prioritypooling = false;
	initializeThreads();
	start();
}

ThreadPool::ThreadPool(int initThreads, int maxThreads, int lowp, int highp)
{
	if (lowp > highp) {
		throw "Low Priority should be less than Highest Priority";
	}
	m_initThreads = initThreads;
	m_maxThreads = maxThreads;
	m_lowp = lowp;
	m_highp = highp;
	m_runFlag = false;
	m_joinComplete = false;
	m_prioritypooling = true;
	initializeThreads();
}

ThreadPool::ThreadPool(int initThreads, int maxThreads)
{
    m_lowp = -1;
    m_highp = -1;
    m_initThreads = initThreads;
    m_maxThreads = maxThreads;
    m_runFlag = false;
    m_joinComplete = false;
    m_prioritypooling = false;
	initializeThreads();
}

void ThreadPool::initializeThreads()
{
    if(m_runFlag) {
        return;
    }
	m_wpool = new TaskPool;
	m_tpool = new std::vector<PoolThread*>;
	for (int i = 0; i < m_initThreads; ++i) {
		PoolThread *thread = new PoolThread();
		thread->execute();
		m_tpool->push_back(thread);
	}
	m_runFlag = true;
	m_poller = new Thread(&ThreadPool::poll, this);
	m_wpool->start();
	m_pollerStarted = false;
	m_complete = false;
	m_mutex = new Mutex;
}

void ThreadPool::start()
{
	if(m_pollerStarted) {
	    return;
	}
	m_poller->execute();
	m_pollerStarted = true;
}

void* ThreadPool::poll(void *arg)
{
	ThreadPool* ths = static_cast<ThreadPool*>(arg);
	assert(ths != NULL);
	ths->m_mutex->lock();
	bool fl = ths->m_runFlag;
	ths->m_mutex->unlock();
	while (fl) {
		if (!ths->m_prioritypooling && ths->m_wpool->tasksPending()) {
			Task *task = ths->m_wpool->getTask();
			if (task != NULL) {
				ths->submit(task);
			}
		}
		else if (ths->m_prioritypooling && ths->m_wpool->tasksPPending()) {
			Task *task = ths->m_wpool->getPTask();
			if (task != NULL) {
				ths->submit(task);
			}
		}
		Thread::mSleep(1);
		ths->m_mutex->lock();
		fl = ths->m_runFlag;
		ths->m_mutex->unlock();
	}
	ths->m_mutex->lock();
	ths->m_complete = true;
	ths->m_mutex->unlock();
	return NULL;
}

void ThreadPool::submit(Task *task)
{
	bool flag = true;
	while (flag) {
		for (size_t var = 0; var < m_tpool->size(); ++var) {
			if (m_tpool->at(var)->isIdle()) {
			    m_tpool->at(var)->checkout(task);
				flag = false;
				break;
			}
		}
		Thread::mSleep(1);
	}
}

void ThreadPool::joinAll()
{
	while (!m_joinComplete) {
		if (!m_prioritypooling) {
			while (m_wpool->tasksPending()) {
				Thread::sSleep(1);
			}
		}
		else {
			while (m_wpool->tasksPPending()) {
				Thread::sSleep(1);
			}
		}
		int i = 0;
		for (size_t var = 0; var < m_tpool->size(); ++var) {
			if (m_tpool->at(var)->isIdle()) {
				++i;
			}
		}
		if (i == m_initThreads) {
		    m_joinComplete = true;
			break;
		}
		else {
			Thread::sSleep(1);
		}
	}
}

void ThreadPool::execute(Task *task, int priority)
{
    if (task != NULL) {
        task->m_tunit = -1;
        task->m_type = -1;
        task->m_priority = priority;
        if (!m_prioritypooling) {
            m_wpool->addTask(task);
        }
        else {
            m_wpool->addPTask(task);
        }
    }
}

void ThreadPool::execute(Task &task, int priority)
{
	task.m_tunit = -1;
	task.m_type = -1;
	task.m_priority = priority;
	if (!m_prioritypooling) {
	    m_wpool->addTask(task);
	}
	else {
	    m_wpool->addPTask(task);
	}
}

void ThreadPool::execute(Task *task)
{
    if (task != NULL) {
        task->m_tunit = -1;
        task->m_type = -1;
        task->m_priority = -1;
        if (!m_prioritypooling) {
            m_wpool->addTask(task);
        }
        else {
            m_wpool->addPTask(task);
        }
    }
}

void ThreadPool::execute(Task &task)
{
	task.m_tunit = -1;
	task.m_type = -1;
	task.m_priority = -1;
	if (!m_prioritypooling) {
	    m_wpool->addTask(task);
	}
	else {
	    m_wpool->addPTask(task);
	}
}

void ThreadPool::schedule(Task *task, long long tunit, int type)
{
    if (task != NULL) {
        task->m_tunit = tunit;
        task->m_type = type;
        task->m_priority = -1;
        if (!m_prioritypooling) {
            m_wpool->addTask(task);
        }
        else {
            m_wpool->addPTask(task);
        }
    }
}

void ThreadPool::schedule(Task &task, long long tunit, int type)
{
	task.m_tunit = tunit;
	task.m_type = type;
	task.m_priority = -1;
	if (!m_prioritypooling) {
	    m_wpool->addTask(task);
	}
	else {
	    m_wpool->addPTask(task);
	}
}

ThreadPool::~ThreadPool()
{
	while(!m_joinComplete) {
		joinAll();
		Thread::mSleep(1);
	}
	this->m_mutex->lock();
	this->m_runFlag = false;
	this->m_mutex->unlock();

	m_mutex->lock();
	bool fl = this->m_complete;
	m_mutex->unlock();
	while(!fl) {
		m_mutex->lock();
		fl = this->m_complete;
		m_mutex->unlock();
		Thread::mSleep(1);
	}
	delete m_poller;
	delete m_wpool;
	for (size_t i = 0; i < m_tpool->size(); ++i) {
		delete m_tpool->at(i);
	}
	delete m_mutex;
}

} // namespace TTP
