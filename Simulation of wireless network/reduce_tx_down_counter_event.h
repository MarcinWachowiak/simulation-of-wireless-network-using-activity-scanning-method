#ifndef REDUCE_TX_DOWN_COUNTER_EVENT_H_
#define REDUCE_TX_DOWN_COUNTER_EVENT_H_

#include "event.h"
#include "transmitter.h"

//Class representing conditional event of reducing the
//current channel clean time when the channel is empty,
// flag is_probing enabled is active and the TX has
// a memory log of when last time checked the channel
// was empty too. Then it reduces the down counter by
// check interval 0,5ms (5 units)

class ReduceTXDownCounterEvent :
	public Event
{
public:
	ReduceTXDownCounterEvent(int time, Simulation* simulation, Transmitter* transmitter);
	~ReduceTXDownCounterEvent();
	
	void execute() override;
	bool isTriggered() override;
private:
	//ptr to affected TX
	Transmitter* transmitter_;

protected:
	
};

#endif

