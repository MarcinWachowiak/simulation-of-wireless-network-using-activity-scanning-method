#include "channel_empty_event.h"
#include "simulation.h"

//Class representing conditional event of empty channel;

ChannelEmptyEvent::ChannelEmptyEvent(int time,
	Simulation* simulation,
	Channel* channel
) : Event(time, simulation),
	channel_(channel)
{
}


ChannelEmptyEvent::~ChannelEmptyEvent()
{
}

bool ChannelEmptyEvent::isTriggered()
{
	if(channel_->is_update_enabled() && channel_->get_transmissions().empty())
	{
		return true;
	} else
	{
		return false;
	}
}

void ChannelEmptyEvent::execute()
{
	try
	{
		//reset flags after channel has been emptied
		channel_->set_is_update_enabled(false);
		channel_->set_is_channel_busy(false);
		channel_->set_was_transmit_error(false);
		simulation_->get_logger()->debug("[{0}]\tChannel empty!", simulation_->get_sim_clock());
	} catch (...)
	{
		simulation_->get_logger()->error("[{0}]\tError while handling ChannelEmptyEvent", simulation_->get_sim_clock());
	}
	
}
