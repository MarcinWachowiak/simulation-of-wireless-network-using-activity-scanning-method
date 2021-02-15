#include "generate_packet_event.h"
#include "simulation.h"

//Class representing an event of packet generation
//automatically reschedules it self to keep continuity
//in random intervals of time adds a packet to local queue
//with timestamp parameter

GeneratePacketEvent::GeneratePacketEvent(
	int time,
	Simulation* simulation,
	Transmitter* transmitter,
	RandomNumGen* generate_packet_rng)
	: Event(time,simulation),
	transmitter_ (transmitter),
	generate_packet_rng_(generate_packet_rng)
{
}

GeneratePacketEvent::~GeneratePacketEvent()
{
}

void GeneratePacketEvent::execute()
{
	try
	{
		//generate new packet with proper timestamp
		auto new_packet = new Packet(simulation_->get_sim_clock());
		//add the packet to the specified transmitter
		transmitter_->get_buffer().push(new_packet);
		
		int next_event_time = generate_packet_rng_->randExpRoundedInt(simulation_->get_k_packet_rng_intensity()); 
		//set the next event occurence on generated time
		time_ = next_event_time;
		//report successful packet generation
		simulation_->get_logger()->debug("[{0}]\tGenerated packet for TX id:{1}",simulation_->get_sim_clock(), transmitter_->get_k_tx_id());
	} catch (...)
	{
		//report error during packet generation
		simulation_->get_logger()->error("[{0}]\tFailed to generate packet for TX id:{1}", simulation_->get_sim_clock(), transmitter_->get_k_tx_id());
	}
}

