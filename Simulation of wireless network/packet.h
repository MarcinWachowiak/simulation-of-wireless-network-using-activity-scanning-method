#ifndef PACKET_H_
#define PACKET_H_

//Class representing data packet
//ready to be sent over the network

class Packet
{
public:
	Packet(size_t gen_timestamp);
	~Packet();

	//setters and getters
	size_t get_gen_timestamp() const;
	void set_gen_timestamp(size_t gen_timestamp);
	
private:
	//variable marking a moment in time whe packet was generated
	//used in packet delay and await calculations
	size_t gen_timestamp;

protected:

	
};
#endif//PACKET_H_

