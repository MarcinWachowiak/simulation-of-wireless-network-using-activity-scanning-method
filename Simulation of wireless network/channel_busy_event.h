#ifndef CHANNEL_BUSY_EVENT_H_
#define CHANNEL_BUSY_EVENT_H_

//Class representing conditional event of busy channel;

#include "event.h"
#include "channel.h"

class ChannelBusyEvent :
	public Event
{
public:
	ChannelBusyEvent(
		int time,
		Simulation* simulation,
		Channel* channel);
	~ChannelBusyEvent();
	bool isTriggered() override;
	void execute() override;
private:
	//ptr to channel to count transmissions
	Channel* channel_;
protected:
	
};

#endif
