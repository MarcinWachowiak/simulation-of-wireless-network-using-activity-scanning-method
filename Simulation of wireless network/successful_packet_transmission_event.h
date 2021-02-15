#ifndef SUCCESSFUL_PACKET_TRANSMISSION_EVENT_H_
#define SUCCESSFUL_PACKET_TRANSMISSION_EVENT_H_

//Event representing a conditional event of removing packet
//due to receiving ACK (successful transmission)
//packet is removed, proper statistics are saved
//and TX is reset to enable handling of any other packets

#include "event.h"
#include "transmitter.h"

class EndAckListeningEvent;

class SuccessfulPacketTransmissionEvent :
	public Event
{
public:
	SuccessfulPacketTransmissionEvent(int time, Simulation* simulation, Transmitter* transmitter, EndAckListeningEvent* end_ack_listening_event);
	~SuccessfulPacketTransmissionEvent();
	
	void execute() override;
	bool isTriggered() override;
private:
	//ptr to transmitter instance
	Transmitter* transmitter_;
	//ptr to event instance
	EndAckListeningEvent* end_ack_listening_event_;
protected:

	
};

#endif

