#include "mark_empty_channel_state_event.h"
#include "simulation.h"

//Class representing conditional event of marking
//channel state (remembering it) to start and measure
//empty period 

MarkEmptyChannelStateEvent::MarkEmptyChannelStateEvent(
	int time, 
	Simulation* simulation, 
	Transmitter* transmitter) :
	Event(time, simulation),
	transmitter_(transmitter)
{
}

MarkEmptyChannelStateEvent::~MarkEmptyChannelStateEvent()
{
}

void MarkEmptyChannelStateEvent::execute()
{
	try
	{
		//raise empty channel memory flag
		transmitter_->set_was_channel_empty(true);

		simulation_->get_logger()->debug("[{0}]\tChannel empty, marking state and starting count down for TX id:{1}, val: {2}", simulation_->get_sim_clock(), transmitter_->get_k_tx_id(), transmitter_->get_curr_cln_time_down_counter());
		transmitter_->set_is_probing_active(false);
	} catch (...)
	{
		simulation_->get_logger()->error("[{0}]\tError while handling MarkEmptyChannelStateEvent", simulation_->get_sim_clock());
	}
	
}

bool MarkEmptyChannelStateEvent::isTriggered()
{
	if (transmitter_->is_probing_active() && !transmitter_->was_channel_empty() && !simulation_->get_wireless_network()->get_channel()->is_channel_busy())
	{
		return true;
	}
	return false;
}
