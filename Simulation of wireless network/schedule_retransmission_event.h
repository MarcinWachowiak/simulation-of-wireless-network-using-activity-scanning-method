#ifndef SCHEDULE_RETRANSMISSION_EVENT_H_
#define SCHEDULE_RETRANSMISSION_EVENT_H_

#include "event.h"
#include "random_num_gen.h"
#include "transmitter.h"

//Class representing a conditional event of scheduling
//retransmission after not receiving ack when listening for it is finished
//and retransmit is possible, current retransmission number is below the maximum value

class EndTxSleepEvent;
class ProbeChannelStateEvent;

class ScheduleRetransmissionEvent :
	public Event
{
public:
	ScheduleRetransmissionEvent(int time, Simulation* simulation, Transmitter* transmitter, RandomNumGen* sleep_time_rng, EndTxSleepEvent* end_tx_sleep_event, ProbeChannelStateEvent* probe_channel_state_event);
	~ScheduleRetransmissionEvent();
	void execute() override;
	bool isTriggered() override;
private:
	//ptr to transmitter instance
	Transmitter* transmitter_;
	//ptr to individual sleep time RNG
	RandomNumGen* sleep_time_rng_;
	//ptr to time event of sleeping
	EndTxSleepEvent* end_tx_sleep_event_;
	//
	ProbeChannelStateEvent* probe_channel_state_event_;
protected:
};

#endif
