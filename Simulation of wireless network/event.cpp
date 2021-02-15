#include "event.h"

//parent class of all events in simulation
//mentions basic handles to check if event
//is triggered by time or conditions and execute it

Event::Event(
	int time,
	Simulation* simulation)
	: time_(time),
	simulation_(simulation)
{
}

Event::~Event()
{
}
//method checking if conditional criteria are met to trigger the event execution
bool Event::isTriggered()
{
	return false;
}

//method checking if event is scheduled
//and its timers should be updated
bool Event::isActive()
{
	if(time_>0)
	{
		return true;
	} else
	{
		return false;
	}
}

//setters and getters
int& Event::get_time()
{
	return time_;
}

void Event::set_time(int time)
{
	time_ = time;
}


