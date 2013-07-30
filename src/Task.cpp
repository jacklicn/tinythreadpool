/*
 *  Project   : TinyThreadPool
 *  File      : Task.cpp
 *  Author    : Your Name
 *  Copyright : GPLv2
 */

#include "Task.h"

namespace TTP
{

Task::Task()
{
    m_tunit = -1;
    m_type = -1;
    m_priority = -1;
}

Task::Task(int priority)
{
    m_tunit = -1;
    m_type = -1;
    m_priority = priority;
}

Task::Task(int tunit, int type)
{
    m_tunit = tunit;
    m_type = type;
    m_priority = -1;
}

Task::~Task()
{
}

bool Task::isWaitOver(Timer *timer)
{
	bool flag = false;
	if (m_type == TimeUnit::NANOSECONDS)
	{
		long long diff = timer->elapsedNanoSeconds();
		if(m_tunit <= diff)
		{
			flag = true;
		}
	}
	else if (m_type == TimeUnit::MICROSECONDS)
	{
		long long diff = timer->elapsedMicroSeconds();
		if(m_tunit <= diff)
		{
			flag = true;
		}
	}
	else if (m_type == TimeUnit::MILLISECONDS)
	{
		long long diff = timer->elapsedMilliSeconds();
		if(m_tunit <= diff)
		{
			flag = true;
		}
	}
	else if (m_type == TimeUnit::SECONDS)
	{
		long long diff = timer->elapsedSeconds();
		if(m_tunit <= diff)
		{
			flag = true;
		}
	}
	else if (m_type == TimeUnit::MINUTES)
	{
		long long diff = timer->elapsedSeconds() / 60;
		diff = diff/60;
		if(m_tunit <= diff)
		{
			flag = true;
		}
	}
	else if (m_type == TimeUnit::HOURS)
	{
		long long diff = timer->elapsedSeconds();
		diff = diff/3600;
		if(m_tunit <= diff)
		{
			flag = true;
		}
	}
	else if(m_type==TimeUnit::DAYS)
	{
		long long diff = timer->elapsedSeconds();
		diff = diff/86400;
		if(m_tunit <= diff)
		{
			flag = true;
		}
	}
	return flag;
}

} // namespace TTP
