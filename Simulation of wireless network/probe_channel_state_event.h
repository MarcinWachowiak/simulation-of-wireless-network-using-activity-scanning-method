#ifndef PROBE_CHANNEL_STATE_EVENT_H_
#define PROBE_CHANNEL_STATE_EVENT_H_

#include "event.h"
#include "transmitter.h"

//Class representing regular time event
//of probing the state of the channel in regular
//time intervals od 0,5ms (5 units)

class ProbeChannelStateEvent :
	public Event
{
public:
	ProbeChannelStateEvent(int time, Simulation* simulation, Transmitter* transmitter);
	~ProbeChannelStateEvent();
	void execute() override;
private:
	//ptr to affected TX
	Transmitter* transmitter_;
protected:
};

#endif
