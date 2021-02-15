#ifndef REMOVE_LOST_PACKET_EVENT_H_
#define REMOVE_LOST_PACKET_EVENT_H_

//Class representing event of removing packet
//after all attempts to transmit it failed

#include "event.h"
#include  "transmitter.h"

class RemoveLostPacketEvent :
	public Event
{
public:
	RemoveLostPacketEvent(int time, Simulation* simulation, Transmitter* transmitter);
	~RemoveLostPacketEvent();
	bool isTriggered() override;
	void execute() override;
private:
	//ptr to transmitter instance
	Transmitter* transmitter_;
protected:

	
};

#endif

