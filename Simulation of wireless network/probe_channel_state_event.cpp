#include "probe_channel_state_event.h"
#include "simulation.h"

//Class representing regular time event
//of probing the state of the channel in regular
//time intervals od 0,5ms (5 units)

ProbeChannelStateEvent::ProbeChannelStateEvent(
	int time,
	Simulation* simulation,
	Transmitter* transmitter) :
	Event(time,simulation),
	transmitter_(transmitter)
{
}

ProbeChannelStateEvent::~ProbeChannelStateEvent()
{
}

void ProbeChannelStateEvent::execute()
{
	try
	{
		//enable execution of conditional events based on channel state
		transmitter_->set_is_probing_active(true);
		//reschedule the event (0,5ms, 5 units)
		time_ = simulation_->get_channel_check_interval();
		simulation_->get_logger()->debug("[{0}]\tChannel probed by TX: {1}", simulation_->get_sim_clock(),transmitter_->get_k_tx_id());
	} catch (...) 
	{
		simulation_->get_logger()->error("[{0}]\tError while handling ProbeChannelStateEvent", simulation_->get_sim_clock());
	}
	
}
