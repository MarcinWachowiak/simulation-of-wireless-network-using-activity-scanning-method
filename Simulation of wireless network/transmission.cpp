#include "transmission.h"

//Class representing an ongoing transmission
//It is equipped with flags marking whether it is a packet or ack,
//additional pointers to RX/TX and is placed in the channel for
//specified time interval

Transmission::Transmission(
	int tran_time, 
	bool is_packet, 
	Transmitter* transmitter, 
	Receiver* receiver)
	: tran_time_(tran_time),
	is_packet_(is_packet),
	transmitter_(transmitter),
	receiver_(receiver)
{
}

Transmission::~Transmission()
{
}

//setters and getters
int Transmission::get_tran_time() const
{
	return tran_time_;
}

void Transmission::set_tran_time(int tran_time)
{
	tran_time_ = tran_time;
}

bool Transmission::is_packet() const
{
	return is_packet_;
}

void Transmission::set_is_packet(bool is_packet)
{
	is_packet_ = is_packet;
}

Transmitter*& Transmission::get_transmitter()
{
	return transmitter_;
}

void Transmission::set_transmitter(Transmitter* transmitter)
{
	transmitter_ = transmitter;
}

Receiver*& Transmission::get_receiver()
{
	return receiver_;
}

void Transmission::set_receiver(Receiver* receiver)
{
	receiver_ = receiver;
}


