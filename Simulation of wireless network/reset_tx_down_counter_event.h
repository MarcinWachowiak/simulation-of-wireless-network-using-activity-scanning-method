#ifndef RESET_TX_DOWN_COUNTER_EVENT_H_
#define RESET_TX_DOWN_COUNTER_EVENT_H_

//Class representing an event of resetting the
//clean time down counter after probing and detecting
//that it is busy

#include "event.h"
#include "transmitter.h"

class ResetTXDownCounterEvent : public Event
{
public:

	ResetTXDownCounterEvent(int time, Simulation* simulation, Transmitter* transmitter);
	~ResetTXDownCounterEvent();

	void execute() override;
	bool isTriggered() override;
	
private:
	//ptr to affected TX
	Transmitter* transmitter_;
protected:
};

#endif

