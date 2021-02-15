#include "wireless_network.h"
#include "simulation.h"

//Class containing all elements of wireless network taking part
//in functioning of the system limited to single channel 

WirelessNetwork::WirelessNetwork(int rx_tx_count, int max_retran_count, Statistics* statistics, int kTxInitDownCounterVal,int kAckMsgTime = 10)
	: kRxTxCount_(rx_tx_count),
	kAckMsgTime_(kAckMsgTime),
	kTxInitDownCounterVal_(kTxInitDownCounterVal),
	statistics_(statistics)
{
	//composing wireless network
	for(int i = 0; i<rx_tx_count; i++)
	{
		//generate pairs TX RX
		auto* loc_receiver = new Receiver(kAckMsgTime,i);
		auto* loc_transmitter = new Transmitter(i, max_retran_count, kTxInitDownCounterVal_, loc_receiver);
		//place them in vect in network
		this->get_receivers().push_back(loc_receiver);
		this->get_transmitters().push_back(loc_transmitter);
	}

	auto* loc_chanel = new Channel();
	channel_ = loc_chanel;
}

WirelessNetwork::~WirelessNetwork()
{
	
	for (unsigned int i = 0; i < transmitters_.size(); ++i)
	{
		delete transmitters_[i];
	}

	for (unsigned int i = 0; i < receivers_.size(); ++i)
	{
		delete receivers_[i];
	}

	delete channel_;
	delete statistics_;
}

void WirelessNetwork::reset()
{
	for (Transmitter* transmitter : transmitters_)
	{
		transmitter->reset();
	}

	for(Receiver* receiver : receivers_)
	{
		receiver->reset();
	}
	
	this->get_statistics()->reset();
	channel_->reset();
}

int WirelessNetwork::get_k_ack_msg_time() const
{
	return kAckMsgTime_;
}


int WirelessNetwork::get_k_rx_tx_count() const
{
	return kRxTxCount_;
}

std::vector<Transmitter*>& WirelessNetwork::get_transmitters()
{
	return transmitters_;
}

std::vector<Receiver*>& WirelessNetwork::get_receivers()
{
	return receivers_;
}


Channel* WirelessNetwork::get_channel() const
{
	return channel_;
}

void WirelessNetwork::set_channel(Channel* channel)
{
	channel_ = channel;
}

Statistics* WirelessNetwork::get_statistics() const
{
	return statistics_;
}

void WirelessNetwork::set_statistics(Statistics* statistics)
{
	statistics_ = statistics;
}

const int& WirelessNetwork::get_k_tx_init_down_counter_val() const
{
	return kTxInitDownCounterVal_;
}

