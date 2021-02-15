#include "channel.h"
#include <algorithm>

Channel::~Channel()
{
	for (Transmission* transmission : transmissions_)
	{
		delete transmission;
	}
	transmissions_.erase(transmissions_.begin(), transmissions_.end());
}

Channel::Channel()
	: is_update_enabled_(false),
	is_channel_busy_(false),
	was_transmit_error_(false)
{
}

//function removing the transmission form channel vector its pointer 
void Channel::removeTranFromChannel(Transmission* transmission_ptr)
{
	std::vector<Transmission*>::iterator position = std::find(transmissions_.begin(), transmissions_.end(), transmission_ptr);
	if (position != transmissions_.end()) // == myVector.end() means the element was not found
		transmissions_.erase(position);
}

//restore to defaults
//remove all transmissions
void Channel::reset()
{
	is_channel_busy_ = false;
	was_transmit_error_ = false;
	is_update_enabled_ = false;

	for (Transmission* transmission : transmissions_)
	{
		delete transmission;
	}
	transmissions_.erase(transmissions_.begin(), transmissions_.end());
}

bool Channel::is_update_enabled() const
{
	return is_update_enabled_;
}

void Channel::set_is_update_enabled(bool is_update_enabled)
{
	is_update_enabled_ = is_update_enabled;
}

//setters and getters
bool Channel::is_channel_busy() const
{
	return is_channel_busy_;
}

void Channel::set_is_channel_busy(bool is_channel_busy)
{
	is_channel_busy_ = is_channel_busy;
}

bool Channel::was_transmit_error() const
{
	return was_transmit_error_;
}

void Channel::set_was_transmit_error(bool was_transmit_error)
{
	was_transmit_error_ = was_transmit_error;
}

std::vector<Transmission*>& Channel::get_transmissions()
{
	return transmissions_;
}
