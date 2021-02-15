#include "receiver.h"

//Class representing physical instance of receiver
//in the network, it is responsible for verification
//of packet consistency, sending ACK transmission
//and collecting statistics about received packets

Receiver::Receiver(int ack_msg_time, int rx_id)
	:
	kRxId_(rx_id),
	was_packet_received_(false),
	was_packet_uncorrupted_(false),
	was_uncorrupted_by_channel_(false),
	coupled_transmitter_(nullptr)
{
}

Receiver::~Receiver()
{
}
//reset to initial state
void Receiver::reset()
{
	was_packet_uncorrupted_ = false;
	was_packet_received_ = false;
	was_uncorrupted_by_channel_ = false;
}

//setters and getters
int Receiver::get_k_rx_id() const
{
	return kRxId_;
}

void Receiver::set_was_packet_received(bool was_packet_received)
{
	this->was_packet_received_ = was_packet_received;
}

void Receiver::set_was_packet_uncorrupted(bool was_packet_uncorrupted)
{
	this->was_packet_uncorrupted_ = was_packet_uncorrupted;
}

bool Receiver::was_uncorrupted_by_channel() const
{
	return was_uncorrupted_by_channel_;
}

void Receiver::set_was_uncorrupted_by_channel(bool was_uncorrupted_by_channel)
{
	was_uncorrupted_by_channel_ = was_uncorrupted_by_channel;
}


bool& Receiver::was_packet_received()
{
	return was_packet_received_;
}

bool& Receiver::was_packet_uncorrupted()
{
	return was_packet_uncorrupted_;
}

Transmitter*& Receiver::get_coupled_transmitter()
{
	return coupled_transmitter_;
}

void Receiver::set_coupled_transmitter(Transmitter* coupled_transmitter)
{
	coupled_transmitter_ = coupled_transmitter;
}

