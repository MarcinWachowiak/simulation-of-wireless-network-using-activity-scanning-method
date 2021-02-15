#ifndef WIRELESS_NETWORK_H_
#define WIRELESS_NETWORK_H_

#include <vector>
#include "channel.h"
#include "receiver.h"
#include "statistics.h"
#include "transmitter.h"

//Class containing all elements of wireless network taking part
//in functioning of the system limited to single channel

class Simulation;

class WirelessNetwork
{
public:
	WirelessNetwork(int rx_tx_count, int max_retran_count, Statistics* statistics,
	                int kTxInitDownCounterVal, int kAckMsgTime);
	~WirelessNetwork();
	
	void reset();

	//setters and getters
	int get_k_rx_tx_count() const;
	int get_k_ack_msg_time() const;
	std::vector<Transmitter*>& get_transmitters();
	std::vector<Receiver*>& get_receivers();
	Channel* get_channel() const;
	void set_channel(Channel* channel);
	Statistics* get_statistics() const;
	void set_statistics(Statistics* statistics);
	const int& get_k_tx_init_down_counter_val() const;

private:
	//number of receivers and transmitters in the system (rx count eq tx count)
	const int kRxTxCount_;
	//sets length of ACK transmission
	//set to 1ms (CTIZ);
	const int kAckMsgTime_;
	//initial value of TX downcounter used in reset and instantiation
	const int kTxInitDownCounterVal_;
	//vector of pointers to transmitter instances
	std::vector <Transmitter*> transmitters_;
	//vector of pointers to receiver instances
	std::vector <Receiver*> receivers_;
	//ptr to single channel instance
	Channel * channel_; 
	//ptr to simulation results containing object
	Statistics* statistics_;
protected:
};
#endif //WIRELESS_NETWORK_H_
