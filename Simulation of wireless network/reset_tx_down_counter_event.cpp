#include "reset_tx_down_counter_event.h"
#include "simulation.h"

//Class representing an event of resetting the
//clean time down counter after probing and detecting
//that it is busy

ResetTXDownCounterEvent::ResetTXDownCounterEvent(
	int time,
	Simulation* simulation, 
	Transmitter* transmitter
) : Event(time,simulation),
	transmitter_(transmitter)
{
}

ResetTXDownCounterEvent::~ResetTXDownCounterEvent()
{
}

void ResetTXDownCounterEvent::execute()
{
	try
	{
		//reset empty channel memory flag
		transmitter_->set_was_channel_empty(false);
		//reset transmitter down counter to default value
		transmitter_->set_curr_cln_time_down_counter(transmitter_->get_k_tx_init_down_counter_val());
		//reset probing
		transmitter_->set_is_probing_active(false);

		simulation_->get_logger()->debug("[{0}]\tChannel busy, counter reset for TX id:{1} to val: {2}", simulation_->get_sim_clock(), transmitter_->get_k_tx_id(), transmitter_->get_k_tx_init_down_counter_val());
	} catch (...)
	{
		simulation_->get_logger()->error("[{0}]\tError while handling ResetTXDownCounterEvent", simulation_->get_sim_clock());
	}
}
	

bool ResetTXDownCounterEvent::isTriggered()
{
	if(transmitter_->is_probing_active() && transmitter_->is_active() && !transmitter_->is_busy() && simulation_->get_wireless_network()->get_channel()->is_channel_busy())
	{
		return true;
	}
	return false;
}
