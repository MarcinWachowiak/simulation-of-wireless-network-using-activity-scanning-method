#include "simulation.h"
#include "start_channel_observing_event.h"
#include "probe_channel_state_event.h"

//Class representing a conditional event if start of channel observing
//if the TX has a packet to send and is active

StartChannelObservingEvent::StartChannelObservingEvent(
	int time,
	Simulation* simulation,
	Transmitter* transmitter,
	ProbeChannelStateEvent* probe_channel_state_event) :
	Event(time, simulation),
	transmitter_(transmitter),
	probe_channel_state_event_(probe_channel_state_event)
{
}

StartChannelObservingEvent::~StartChannelObservingEvent()
{
}

bool StartChannelObservingEvent::isTriggered()
{
	if(!transmitter_->is_channel_observed() && transmitter_->is_active() && !transmitter_->is_busy() && !transmitter_->get_buffer().empty())
	{
		return true;
	} else
	{
		return false;
	}
}

void StartChannelObservingEvent::execute()
{
	try
	{
		//mark that TX started channel observing and probing
		transmitter_->set_is_channel_observed(true);
		//schedule the probing of state and first conditional decisions
		//related to channel state to the next closest moment in time
		//(not simultaneously after receiving packet)
		probe_channel_state_event_->set_time(simulation_->get_minimal_time_step());
		simulation_->get_logger()->debug("[{0}]\tStarting channel observing and probing for TX:{1}", simulation_->get_sim_clock(),transmitter_->get_k_tx_id());
	} catch (...)
	{
		simulation_->get_logger()->error("[{0}]\tError while handling StartChannelObservingEvent", simulation_->get_sim_clock());
	}
	
}
