#ifndef RECEIVER_H_
#define RECEIVER_H_

//Class representing physical instance of receiver
//in the network, it is responsible for verification
//of packet consistency, sending ACK transmission
//and collecting statistics about received packets

class Transmitter;

class Receiver
{
public:
	Receiver(int ack_msg_time, int rx_id);
	~Receiver();

	//restore variables to default
	void reset();
	
	//setters and getters
	int get_k_rx_id() const;
	void set_was_packet_received(bool was_packet_received);
	void set_was_packet_uncorrupted(bool was_packet_uncorrupted);
	bool was_uncorrupted_by_channel() const;
	void set_was_uncorrupted_by_channel(bool was_uncorrupted_by_channel);
	bool& was_packet_received();
	bool& was_packet_uncorrupted();
	Transmitter*& get_coupled_transmitter();
	void set_coupled_transmitter(Transmitter* coupled_transmitter);

private:
	//receiver unique identification number
	const int kRxId_;
	//was any packet received (corrupted or not)
	bool was_packet_received_;
	//was there a collision flag
	bool was_packet_uncorrupted_;
	//was there channel error flag
	bool was_uncorrupted_by_channel_;
	//coupled transmitter to notify with ACK
	//set in each end transmission 
	Transmitter* coupled_transmitter_;
protected:
};
#endif //RECEIVER_H_