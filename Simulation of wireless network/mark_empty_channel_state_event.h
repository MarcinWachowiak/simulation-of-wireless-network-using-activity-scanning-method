#ifndef MARK_EMPTY_CHANNEL_STATE_EVENT_H_
#define MARK_EMPTY_CHANNEL_STATE_EVENT_H_

#include "event.h"
#include "transmitter.h"


class MarkEmptyChannelStateEvent :
	public Event
{
public:
	MarkEmptyChannelStateEvent(int time, Simulation* simulation, Transmitter* transmitter);
	~MarkEmptyChannelStateEvent();
	
	void execute() override;
	bool isTriggered() override;

	
private:
	//ptr to affected TX
	Transmitter* transmitter_;
protected:
};

#endif
