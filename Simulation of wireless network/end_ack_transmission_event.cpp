#include "end_ack_transmission_event.h"
#include "simulation.h"

//Class representing end of ACK transmission, triggering proper reaction form TX

EndAckTransmissionEvent::EndAckTransmissionEvent(
	int time, 
	Simulation* simulation, 
	Transmission* transmission) :
	Event(time, simulation),
	transmission_(transmission)
{
}

EndAckTransmissionEvent::~EndAckTransmissionEvent()
{
}

void EndAckTransmissionEvent::execute()
{
	try
	{
		//ack RX->TX
		//transmitter will get next packet, refresh/reset 
		transmission_->get_transmitter()->set_was_tran_ok(true);
		//disable re trigger
		time_ = -1;
		//remove specified from channel's vector of transmissions
		simulation_->get_wireless_network()->get_channel()->removeTranFromChannel(transmission_);
		//delete instance
		simulation_->get_logger()->debug("[{0}]\tUncorrupted ACK received by TX id:{1}", simulation_->get_sim_clock(), transmission_->get_transmitter()->get_k_tx_id());
		
		delete transmission_;

	} catch (...)
	{
		simulation_->get_logger()->error("[{0}]\tError in handling end ACK transmission event", simulation_->get_sim_clock());
	}
	
}

Transmission* EndAckTransmissionEvent::get_transmission() const
{
	return transmission_;
}

void EndAckTransmissionEvent::set_transmission(Transmission* transmission)
{
	transmission_ = transmission;
}
