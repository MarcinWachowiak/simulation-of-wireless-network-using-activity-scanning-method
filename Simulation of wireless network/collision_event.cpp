#include "collision_event.h"
#include "simulation.h"

//Class representing conditional event of packet collision

CollisionEvent::CollisionEvent(
	int time,
	Simulation* simulation,
	Channel* channel
	): Event(time, simulation),
	channel_(channel)
	{
}

CollisionEvent::~CollisionEvent()
{
}

bool CollisionEvent::isTriggered()
{
	if(!channel_->was_transmit_error() && channel_->get_transmissions().size()>1)
	{
		return true;
	}
	return false;
}

void CollisionEvent::execute()
{
	try
	{
		//mark error/collision occurrence
		channel_->set_was_transmit_error(true);
		simulation_->get_logger()->debug("[{0}]\tCollision detected", simulation_->get_sim_clock());
	} catch (...)
	{
		simulation_->get_logger()->error("[{0}]\tError while handling CollisionEvent", simulation_->get_sim_clock());
	}
	
}
