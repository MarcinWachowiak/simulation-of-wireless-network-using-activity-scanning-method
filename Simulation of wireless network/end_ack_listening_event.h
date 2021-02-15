#ifndef END_ACK_LISTENING_EVENT_H_
#define END_ACK_LISTENING_EVENT_H_

#include "event.h"
#include "transmitter.h"

//time event representing end of waiting/listening for ACK

class EndAckListeningEvent :
	public Event
{
public:
	EndAckListeningEvent(int time, Simulation* simulation, Transmitter* transmitter);
	~EndAckListeningEvent();
	void execute() override;
	
private:
	//ptr to transmitter instance
	Transmitter* transmitter_;
protected:
	
};

#endif

