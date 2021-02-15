#include "simulation.h"
#include "update_channel_state_event.h"

//Event representing updating channel state that
//ocurrs only once in single step (single time moment)
//raises flag enabling change of channel state
//added to prevent channel from being simultaneously
//in two or more states (limit the amount of events taking place in the same moment)

UpdateChannelStateEvent::UpdateChannelStateEvent(
	int time, 
	Simulation* simulation, 
	Channel* channel) :
	Event(time,simulation),
	channel_(channel)
{
}

UpdateChannelStateEvent::~UpdateChannelStateEvent()
{
}

void UpdateChannelStateEvent::execute()
{
	try
	{
		//raise flag enabling channel state to be updated
		channel_->set_is_update_enabled(true);
		//schedule the event for the next closest moment in time (minimal time step)
		time_ = simulation_->get_minimal_time_step();
	} catch (...)
	{
		simulation_->get_logger()->error("[{0}]\tError while handling UpdateChannelStateEvent", simulation_->get_sim_clock());
	}
}
