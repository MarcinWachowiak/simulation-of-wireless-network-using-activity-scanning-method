#include "reduce_tx_down_counter_event.h"
#include "simulation.h"

//Class representing conditional event of reducing the
//current channel clean time when the channel is empty,
// flag is_probing enabled is active and the TX has
// a memory log of when last time checked the channel
// was empty too. Then it reduces the down counter by
// check interval 0,5ms (5 units)

ReduceTXDownCounterEvent::ReduceTXDownCounterEvent(
	int time,
	Simulation* simulation,
	Transmitter* transmitter
	): Event(time, simulation),
	   transmitter_(transmitter)

{
}

ReduceTXDownCounterEvent::~ReduceTXDownCounterEvent()
{
}

void ReduceTXDownCounterEvent::execute()
{
	try
	{
		//channel empty
		//reduce counter by simulation step
		transmitter_->get_curr_cln_time_down_counter() -= simulation_->get_channel_check_interval();
		transmitter_->set_is_probing_active(false);

		simulation_->get_logger()->debug("[{0}]\tChannel empty, counter reduced for TX id:{1}, val: {2}", simulation_->get_sim_clock(), transmitter_->get_k_tx_id(), transmitter_->get_curr_cln_time_down_counter());
	}catch (...)
	{
		simulation_->get_logger()->error("[{0}]\tError while handling ReduceTXDownCounterEvent", simulation_->get_sim_clock());
	}
	
}

bool ReduceTXDownCounterEvent::isTriggered()
{
	if (transmitter_->is_probing_active() && transmitter_->is_active() && !transmitter_->is_busy() && transmitter_->was_channel_empty() && !simulation_->get_wireless_network()->get_channel()->is_channel_busy())
	{
		return true;
	}
	return false;
}
