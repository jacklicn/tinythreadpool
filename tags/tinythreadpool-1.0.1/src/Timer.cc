/*
 *  Project   : TinyThreadPool
 *  File      : Timer.cc
 *  Author    : Your Name
 *  Copyright : GPLv2
 */

#include "Timer.h"

namespace TTP
{

Timer::Timer()
{
    m_elap = 0;
}

Timer::~Timer()
{
}

void Timer::start()
{
    m_elap = 0;
	clock_gettime(CLOCK_MONOTONIC, &m_st);
}

long long Timer::getCurrentTime()
{
	timespec en;
	clock_gettime(CLOCK_MONOTONIC, &en);
	return static_cast<long long>((en.tv_sec * 1E9) + en.tv_nsec);
}

long long Timer::getTimestamp()
{
	timespec en;
	clock_gettime(CLOCK_MONOTONIC, &en);
	return static_cast<long long>(en.tv_sec);
}

void Timer::end()
{
	timespec en;
	clock_gettime(CLOCK_MONOTONIC, &en);
	m_elap = static_cast<long long>(((en.tv_sec - m_st.tv_sec) * 1E9) + (en.tv_nsec - m_st.tv_nsec));
}

long long Timer::elapsedMicroSeconds()
{
	timespec en;
	clock_gettime(CLOCK_MONOTONIC, &en);
	return static_cast<long long>((((en.tv_sec - m_st.tv_sec) * 1E9) + (en.tv_nsec - m_st.tv_nsec))/1E3);
}

long long Timer::elapsedMilliSeconds()
{
	timespec en;
	clock_gettime(CLOCK_MONOTONIC, &en);
	return static_cast<long long>((((en.tv_sec - m_st.tv_sec) * 1E9) + (en.tv_nsec - m_st.tv_nsec))/1E6);
}

long long Timer::elapsedNanoSeconds()
{
	timespec en;
	clock_gettime(CLOCK_MONOTONIC, &en);
	return static_cast<long long>(((en.tv_sec - m_st.tv_sec) * 1E9) + (en.tv_nsec - m_st.tv_nsec));
}

long long Timer::elapsedSeconds()
{
	timespec en;
	clock_gettime(CLOCK_MONOTONIC, &en);
	return static_cast<long long>(en.tv_sec - m_st.tv_sec);
}

} // namespace TTP

