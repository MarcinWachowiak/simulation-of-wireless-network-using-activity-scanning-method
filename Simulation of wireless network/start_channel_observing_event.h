#ifndef START_CHANNEL_OBSERVING_H_
#define START_CHANNEL_OBSERVING_H_

//Class representing a conditional event if start of channel observing
//if the TX has a packet to send and is active

#include "event.h"
#include "transmitter.h"

class StartChannelObservingEvent :
	public Event
{
public:
	StartChannelObservingEvent(int time, Simulation* simulation, Transmitter* transmitter,
	                           ProbeChannelStateEvent* probe_channel_state_event);
	~StartChannelObservingEvent();

	bool isTriggered() override;
	void execute() override;
private:
	//ptr to affected TX
	Transmitter* transmitter_;
	//ptr to regular probing
	ProbeChannelStateEvent* probe_channel_state_event_;
protected:
	
};

#endif
