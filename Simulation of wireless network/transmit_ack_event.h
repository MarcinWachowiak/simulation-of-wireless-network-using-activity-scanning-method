#ifndef TRANSMIT_ACK_EVENT_H_
#define TRANSMIT_ACK_EVENT_H_

#include "event.h"

class Simulation;
class EndAckTransmissionEvent;

//Class representing conditional event of sending ACK by RX
//has method checking event trigger conditions
//and handling sending response to the TX
//if reception was successful

class TransmitAckEvent :
	public Event
{
public:
	TransmitAckEvent(
		int time,
		Simulation* simulation,
		Receiver* receiver,
		EndAckTransmissionEvent* end_ack_transmission_event );
	~TransmitAckEvent();

	void execute() override;
	//additional method to check if event can be executed
	bool isTriggered() override;
private:
	//ptr to executing RX
	Receiver* receiver_;
	//ptr to respective ack end transmission event to set time
	EndAckTransmissionEvent* end_ack_transmission_event_;
protected:
};

#endif
