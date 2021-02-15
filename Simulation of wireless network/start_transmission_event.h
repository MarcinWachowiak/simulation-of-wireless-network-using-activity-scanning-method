#ifndef START_TRANSMISSION_EVENT_H_
#define START_TRANSMISSION_EVENT_H_

#include "event.h"
#include "end_packet_transmission_event.h"
#include "end_ack_listening_event.h"

//Class representing conditional event of starting
//first packet transmission by TX
//first transmit attempt timestamp is saved
//and proper handling of the end and status checking is issued

class StartTransmissionEvent :
	public Event
{
public:
	StartTransmissionEvent(int time, 
	Simulation* simulation, 
	Transmitter* transmitter,
	RandomNumGen* transmission_time_rng,
	EndAckListeningEvent* end_ack_listening_event, 
	EndPacketTransmissionEvent* end_packet_transmission_event,
	ProbeChannelStateEvent* probe_channel_state_event
		);
	~StartTransmissionEvent();

	void execute() override;
	//method checking trigger conditions
	bool isTriggered() override;
private:
	//ptr to affected TX
	Transmitter* transmitter_;
	//ptr to individual transmission time rng
	RandomNumGen* transmission_time_rng_;
	//ptr to associated end transmission event
	EndAckListeningEvent* end_ack_listening_event_;
	EndPacketTransmissionEvent* end_packet_transmission_event_;
	ProbeChannelStateEvent* probe_channel_state_event_;
protected:
	
};

#endif

