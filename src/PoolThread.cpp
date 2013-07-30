/*
 *  Project   : TinyThreadPool
 *  File      : PoolThread.cpp
 *  Author    : Your Name
 *  Copyright : GPLv2
 */

#include <iostream>
#include <exception>
#include <assert.h>
#include "PoolThread.h"

namespace TTP
{

void* PoolThread::run(void *arg)
{
	PoolThread* ths = static_cast<PoolThread*>(arg);
	assert(ths != NULL);
	ths->m_mutex->lock();
	bool fl = ths->m_runFlag;
	ths->m_mutex->unlock();
	while (fl)
	{
		ths->m_thread->wait();
		Task* task = ths->getTask();
		if (task)
		{
			try
			{
				task->run();
			}
			catch(std::exception &e)
			{
		        std::cerr << e.what() << std::endl;
			}
			catch(...)
			{
			    std::cerr << "pool thread catch exception !" << std::endl;
			}
			ths->release();
		}
		ths->m_mutex->lock();
		fl = ths->m_runFlag;
		ths->m_mutex->unlock();
	}
	ths->m_mutex->lock();
	ths->m_complete = true;
	ths->m_mutex->unlock();
	return NULL;
}

PoolThread::PoolThread()
{
    m_task = NULL;
    m_idle = true;
    m_complete = false;
    m_runFlag = true;
    m_thrdStarted = false;
	m_mutex = new Mutex;
	m_thread = new Thread(&run, this);
}

PoolThread::~PoolThread()
{
    m_runFlag = false;
	m_mutex->lock();
	bool fl = m_complete;
	m_mutex->unlock();
	while(!fl)
	{
		m_mutex->lock();
		fl = m_complete;
		m_mutex->unlock();
		m_thread->interrupt();
		Thread::mSleep(1);
	}
	delete m_thread;
	delete m_mutex;
}

void PoolThread::execute()
{
	if(m_thrdStarted)
	{
	    return;
	}
	m_mutex->lock();
	m_thread->execute();
	m_thrdStarted = true;
	m_mutex->unlock();
}

void PoolThread::checkout(Task *task)
{
	m_mutex->lock();
	m_idle = false;
	m_task = task;
	if(m_thrdStarted)
	{
		this->m_thread->interrupt();
	}
	m_mutex->unlock();
}

void PoolThread::release()
{
	m_mutex->lock();
	m_task = NULL;
	m_idle = true;
	m_mutex->unlock();
}

bool PoolThread::isIdle()
{
    bool idle = false;
	m_mutex->lock();
	idle = m_idle;
	m_mutex->unlock();
	return idle;
}

Task* PoolThread::getTask()
{
	m_mutex->lock();
	Task* task = m_task;
	m_mutex->unlock();
	return task;
}

} // namespace TTP

