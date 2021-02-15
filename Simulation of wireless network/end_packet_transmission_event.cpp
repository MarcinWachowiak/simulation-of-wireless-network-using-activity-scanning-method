#include "end_packet_transmission_event.h"
#include "simulation.h"

// class time even of end of packet transmission

EndPacketTransmissionEvent::EndPacketTransmissionEvent(
	int time,
	Simulation* simulation,
	Transmission* transmission,
	RandomNumGen* msg_corrupted_by_channel_rng)
	: Event(time, simulation),
	transmission_(transmission),
	msg_corrupted_by_channel_rng_(msg_corrupted_by_channel_rng)
{
}

EndPacketTransmissionEvent::~EndPacketTransmissionEvent()
{
}

void EndPacketTransmissionEvent::execute()
{
	try
	{
		//set proper reception flags in RX
		transmission_->get_receiver()->set_was_packet_received(true);
		transmission_->get_receiver()->set_was_packet_uncorrupted(!simulation_->get_wireless_network()->get_channel()->was_transmit_error());
		bool was_uncorrupted_flag = msg_corrupted_by_channel_rng_->randZeroOne(simulation_->get_k_channel_probability_of_success());
		transmission_->get_receiver()->set_was_uncorrupted_by_channel(was_uncorrupted_flag);
		//set/update ptr to TX for ACK sending and processing in RX
		transmission_->get_receiver()->set_coupled_transmitter(transmission_->get_transmitter());
		time_ = -1;
		//remove specified from channel's vector of transmissions
		simulation_->get_wireless_network()->get_channel()->removeTranFromChannel(transmission_);
		//delete instance
		simulation_->get_logger()->debug("[{0}]\tEnded packet transmission for RX id:{1}, Was uncorrupted by channel (TER): {2}", simulation_->get_sim_clock(), transmission_->get_receiver()->get_k_rx_id(),was_uncorrupted_flag);
		
		delete transmission_;	
	} catch (...)
	{
		simulation_->get_logger()->error("[{0}]\tError in handling end packet transmission event", simulation_->get_sim_clock());
	}
}

Transmission* EndPacketTransmissionEvent::get_transmission() const
{
	return transmission_;
}

void EndPacketTransmissionEvent::set_transmission(Transmission* transmission)
{
	transmission_ = transmission;
}
