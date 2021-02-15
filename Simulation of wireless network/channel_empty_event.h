#ifndef CHANNEL_EMPTY_EVENT_H_
#define CHANNEL_EMPTY_EVENT_H_

//Class representing conditional event of empty channel;

#include "event.h"
#include "channel.h"

class ChannelEmptyEvent :
	public Event
{
public:
	ChannelEmptyEvent(int time, Simulation* simulation, Channel* channel);
	~ChannelEmptyEvent();
	
	bool isTriggered() override;
	void execute() override;
private:
	//ptr to channel to count transmissions
	Channel* channel_;
protected:
};

#endif