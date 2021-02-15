#ifndef CHANNEL_H_
#define CHANNEL_H_

#include "transmission.h"
//Class representing radio channel
//Stores all transmissions present at the time
//based on the length of transmissions vector and time events
//updates the flags responsible for channel state (busy, error, empty)
//each transmission is removed after its transmit time is finished

class Channel
{
public:
	~Channel();
	Channel();

	//removes transmission from channel vector by a pointer to it
	void removeTranFromChannel(Transmission* transmission_ptr);
	//restore to defaults - clean channel 
	void reset();

	//setters and getters
	bool is_update_enabled() const;
	void set_is_update_enabled(bool is_update_enabled);
	bool is_channel_busy() const;
	void set_is_channel_busy(bool is_channel_busy);
	bool was_transmit_error() const;
	void set_was_transmit_error(bool was_transmit_error);
	std::vector<Transmission*>& get_transmissions();

private:
	//update flag;
	bool is_update_enabled_;
	//flag checking if there are any transmissions present
	//(at least one in the vector)
	bool is_channel_busy_;
	//presistent flag after at least two transmissions were present in the channel
	bool was_transmit_error_;
	//vector of messages: packets or ACK being sent over channel
	std::vector <Transmission*> transmissions_;
	
protected:

};

#endif //CHANNEL_H_
 