#ifndef END_PACKET_TRANSMISSION_EVENT_H_
#define END_PACKET_TRANSMISSION_EVENT_H_

// class time even of end of packet transmission

#include "event.h"
#include "transmission.h"
#include "random_num_gen.h"

class EndPacketTransmissionEvent :
	public Event
{
public:
	EndPacketTransmissionEvent(int time, Simulation* simulation, Transmission* transmission, RandomNumGen* msg_corrupted_by_channel_rng);
	~EndPacketTransmissionEvent();
	
	void execute() override;


	Transmission* get_transmission() const;
	void set_transmission(Transmission* transmission);
private:
	//ptr to transmission that will be affected - deleted
	Transmission* transmission_;
	//ptr to rng generator responsible for 0-1 distribution
	//was transmission interpretation ok (unaffected by channel)
	RandomNumGen* msg_corrupted_by_channel_rng_;
protected:

};

#endif
