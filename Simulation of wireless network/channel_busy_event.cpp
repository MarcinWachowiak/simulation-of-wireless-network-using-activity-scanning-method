#include "channel_busy_event.h"
#include "simulation.h"

//Class representing conditional event of busy channel;

ChannelBusyEvent::ChannelBusyEvent(
	int time, 
	Simulation* simulation, 
	Channel* channel) :
	Event(time, simulation),
	channel_(channel)
{
}

ChannelBusyEvent::~ChannelBusyEvent()
{
}

bool ChannelBusyEvent::isTriggered()
{
	if(channel_->is_update_enabled() && channel_->get_transmissions().size() >= 1)
	{
		return true;
	} else
	{
		return false;
	}
}

void ChannelBusyEvent::execute()
{
	try
	{
		channel_->set_is_update_enabled(false);
		channel_->set_is_channel_busy(true);
		simulation_->get_logger()->debug("[{0}]\tChannel busy detected!", simulation_->get_sim_clock());
	}
	catch (...)
	{
		simulation_->get_logger()->error("[{0}]\tError while handling ChannelBusyEvent", simulation_->get_sim_clock());
	}
}
