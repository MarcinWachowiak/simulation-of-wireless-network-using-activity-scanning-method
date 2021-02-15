#ifndef END_TX_SLEEP_EVENT_H_
#define END_TX_SLEEP_EVENT_H_

#include "event.h"
#include "transmitter.h"

//class representing time event of wake up from sleep
//re enabling of normal TX activity

class EndTxSleepEvent :
	public Event
{
public:
	EndTxSleepEvent(int time, Simulation* simulation, Transmitter* transmitter);
	~EndTxSleepEvent();
	void execute() override;
	
private:
	//ptr to transmitter instance
	Transmitter* transmitter_;
protected:
	
};

#endif
