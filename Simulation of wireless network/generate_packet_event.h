#ifndef GENERATE_PACKET_EVENT_H_
#define GENERATE_PACKET_EVENT_H_

#include "event.h"
#include "transmitter.h"
#include "random_num_gen.h"

//Class representing an event of packet generation
//automatically reschedules it self to keep continuity
//in random intervals of time adds a packet to local queue
//with timestamp parameter

class GeneratePacketEvent :
	public Event
{
public:
	GeneratePacketEvent(int time,
		Simulation* simulation,
		Transmitter* transmmitter,
		RandomNumGen* generate_packet_rng);
	~GeneratePacketEvent();

	void execute() override;

private:
	//ptr to TX that will be affected
	Transmitter* transmitter_;
	//pointer to individual RNG for packet time generation
	RandomNumGen* generate_packet_rng_;

protected:
};

#endif

