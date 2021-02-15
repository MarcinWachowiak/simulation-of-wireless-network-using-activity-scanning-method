#ifndef UPDATE_CHANNEL_STATE_EVENT_H_
#define UPDATE_CHANNEL_STATE_EVENT_H_

#include "event.h"
#include "channel.h"

//Event representing updating channel state that
//ocurrs only once in single step (single time moment)
//raises flag enabling change of channel state
//added to prevent channel from being simultaneously
//in two or more states (limit the amount of events taking place in the same moment)

class UpdateChannelStateEvent :
	public Event
{
public:
	UpdateChannelStateEvent(
		int time, 
		Simulation* simulation, 
		Channel* channel);
	~UpdateChannelStateEvent();
	
	void execute() override;
	
private:
	//ptr to channel
	Channel* channel_;
protected:

	
};

#endif

