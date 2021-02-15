#ifndef TRANSMISSION_H_
#define TRANSMISSION_H_

#include "receiver.h"
#include "transmitter.h"

//Class representing an ongoing transmission
//It is equipped with flags marking whether it is a packet or ack,
//additional pointers to RX/TX and is placed in the channel for
//specified time interval

class Transmission
{
public:
	Transmission(int tran_time, bool is_packet, Transmitter * transmitter, Receiver* receiver);
	~Transmission();

	//setters and getters
	int get_tran_time() const;
	void set_tran_time(int tran_time);
	bool is_packet() const;
	void set_is_packet(bool is_packet);
	Transmitter*& get_transmitter();
	void set_transmitter(Transmitter* transmitter);
	Receiver*& get_receiver();
	void set_receiver(Receiver* receiver);


private:
	//random generated variable defining
	//how long transmission will be present in the channel
	int tran_time_;
	//message type 1-packet, 0-ACK
	//is_packet flag decides the direction of transmission
	//is_packet==1 TX->RX
	//is_packet==0 RX->TX
	bool is_packet_;
	//ptrs to the couple of devices in involved in communication process
	Transmitter* transmitter_;
	Receiver* receiver_;

protected:
};
#endif //TRANSMISSION_H_
