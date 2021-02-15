#ifndef COLLISION_EVENT_H_
#define COLLISION_EVENT_H_

#include "event.h"
#include "channel.h"

//Class representing conditional event of packet collision

class Channel;

class CollisionEvent :
	public Event
{
public:
	CollisionEvent(
		int time,
		Simulation* simulation,
		Channel* channel);
	~CollisionEvent();

	bool isTriggered() override;
	void execute() override;
private:
	//ptr to channel to count transmissions
	Channel* channel_;

protected:
	
};

#endif
