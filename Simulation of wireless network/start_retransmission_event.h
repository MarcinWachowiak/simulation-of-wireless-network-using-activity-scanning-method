#ifndef START_RETRANSMISSION_EVENT_H_
#define START_RETRANSMISSION_EVENT_H_

#include "event.h"
#include "end_packet_transmission_event.h"
#include "end_ack_listening_event.h"

//Class representing conditional event of restarting
//transmission by TX has method checking trigger conditions
//after triggering and starting proper events are planned to
//handle ends of transmission and listening

class StartRetransmissionEvent :
	public Event
{
public:
	StartRetransmissionEvent(int time,
		Simulation* simulation,
		Transmitter* transmitter,
		RandomNumGen* transmission_time_rng,
		EndAckListeningEvent* end_ack_listening_event,
		EndPacketTransmissionEvent* end_packet_transmission_event,
		ProbeChannelStateEvent* probe_channel_state_event
	);
	~StartRetransmissionEvent();

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

