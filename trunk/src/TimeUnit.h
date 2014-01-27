/*
 *  Project   : TinyThreadPool
 *  File      : TimeUnit.h
 *  Author    : Your Name
 *  Copyright : GPLv2
 */

#ifndef TIMEUNIT_H_
#define TIMEUNIT_H_

namespace TTP
{

class TimeUnit
{
public:
	static const int NANOSECONDS = 0;
	static const int MICROSECONDS = 1;
	static const int MILLISECONDS = 2;
	static const int SECONDS = 3;
	static const int MINUTES = 4;
	static const int HOURS = 5;
	static const int DAYS = 6;
};

} // namespace TTP

#endif /* TIMEUNIT_H_ */
