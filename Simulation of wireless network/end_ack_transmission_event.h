#ifndef END_ACK_TRANSMISSION_EVENT_H_
#define END_ACK_TRANSMISSION_EVENT_H_

#include "event.h"
#include "transmission.h"

//Class representing end of ACK transmission, triggering proper reaction form TX

class EndAckTransmissionEvent :
	public Event
{
public:
	EndAckTransmissionEvent(int time, Simulation* simulation, Transmission* transmission);
	~EndAckTransmissionEvent();

	void execute() override;

	Transmission* get_transmission() const;
	void set_transmission(Transmission* transmission);
private:
	//ptr to transmission that will be affected - deleted
	Transmission* transmission_;
protected:
	
};

#endif
