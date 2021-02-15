#include "packet.h"

//Class representing data packet
//ready to be sent over the network

Packet::Packet(size_t gen_timestamp)
	: gen_timestamp(gen_timestamp)
{
}

Packet::~Packet()
{
}

//setters and getters
size_t Packet::get_gen_timestamp() const
{
	return gen_timestamp;
}

void Packet::set_gen_timestamp(size_t gen_timestamp)
{
	this->gen_timestamp = gen_timestamp;
}