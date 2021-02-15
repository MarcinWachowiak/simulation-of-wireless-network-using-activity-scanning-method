#ifndef UPDATE_TX_DOWN_COUNTERS_EVENT_H_
#define UPDATE_TX_DOWN_COUNTERS_EVENT_H_

#include "event.h"
#include "simulation.h"

//Class representing a regular event of updating local TX
//clean channel down counter, depending on a series of conditions
//automatically reschedules itself

class UpdateTxDownCounterEvent :
	public Event
{
public:
	UpdateTxDownCounterEvent(
		int time,
		Simulation* simulation,
		Transmitter* transmitter
	);
	~UpdateTxDownCounterEvent();

	void execute() override;

	//getters and setters
	bool& is_was_channel_free();
	void set_was_channel_free(bool was_channel_free);
	
private:
	//ptr to affected TX
	Transmitter* transmitter_;
	//flag marking if channel was free last time observed
	bool was_channel_free_;
	
protected:
};

#endif
