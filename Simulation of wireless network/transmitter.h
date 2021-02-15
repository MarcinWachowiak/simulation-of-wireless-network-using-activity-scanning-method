#ifndef TRANSMITTER_H_
#define TRANSMITTER_H_

#include <queue>
#include "packet.h"
#include "receiver.h"

class Simulation;

//Class representing a transmitting unit
//It is responsible for storing packets ready to send
//observing the channel and counting it's clean time,
//starting transmission/retransmission removing packet
//and maintaining statistics

class Transmitter
{
public:
	Transmitter(int tx_id, int max_retran_count, int kTxInitDownCounterVal, Receiver* receiver);

	~Transmitter();
	
	//restore to defaults
	void reset();

	//setters and getters
	bool is_busy() const;
	void set_is_busy(bool is_busy);
	int get_k_tx_id() const;
	int get_k_max_retran_count() const;
	int get_k_tx_init_down_counter_val() const;
	std::queue<Packet*>& get_buffer();
	int get_transmission_time() const;
	void set_transmission_time(int transmission_time);
	int& get_curr_cln_time_down_counter();
	void set_curr_cln_time_down_counter(int curr_cln_time_down_counter);
	int& get_curr_retran_num();
	void set_curr_retran_num(int curr_retran_num);
	int get_wait_time_multpl() const;
	void set_wait_time_multpl(int wait_time_multpl);
	bool was_tran_ok() const;
	void set_was_tran_ok(bool was_tran_ok);
	Receiver*& get_coupled_receiver();
	void set_coupled_receiver(Receiver* coupled_receiver);
	bool& was_channel_empty();
	void set_was_channel_empty(bool was_channel_empty);
	bool& is_listening_finished();
	void set_is_listening_finished(bool has_listening_finished);
	bool is_active() const;
	void set_is_active(bool is_active);
	bool is_probing_active() const;
	void set_is_probing_active(bool is_probing_active);
	bool is_channel_observed() const;
	void set_is_channel_observed(bool is_channel_observed);
	size_t& get_transmit_timestamp();
	void set_transmit_timestamp(size_t transmit_timestamp);
	
private:
	//individual identification number
	const int kTxId_;
	//maximum number of permitted retransmissions
	//after when the counter of retransmission attempt
	//is greater than this the packet is removed form queue
	const int kMaxRetranCount_;
	//length of time that channel needs to be clean to start transmission
	const int kTxInitDownCounterVal_;
	//FIFO queue where packets are listed for transmission
	std::queue <Packet*> buffer_;
	//timelength of packet transmission (channel occupation)
	int transmission_time_;
	//loclal timer counting time when channel is empty
	//it is a down counter starting from time:
	//when it is first transmission attempt or sleep time has
	//already passed after failed transmission the its value is equal to
	//(DIFS+smallest time quant) 5ms+0,5ms =5,5ms
	//else (when value is greater than 5,5ms) transmitter is in sleep mode waiting
	//generated interval of time to observe channel again
	//when its value is 0 transmitter should attempt to send packet
	int curr_cln_time_down_counter_;
	//current retransmission number (r)
	//used in sleep time generation
	int curr_retran_num_;
	//variable used to store drawn/generated sleep time multiplier
	int wait_time_multpl_;
	//indicator whether ACK flag was received by transmitter
	bool was_tran_ok_;
	//flag used in excluding the transmitter form regular down counting
	bool is_busy_;
	//pointer to coupler receiver to enable easy transmission setting
	Receiver* coupled_receiver_;
	//flag remembering last state of channel
	bool was_channel_empty_;
	//flag marking finish of ack listening
	bool is_listening_finished_;
	//flag marking state of the TX (active/sleeping)
	bool is_active_;
	//flag marking probing state of the channel
	bool is_probing_active_;
	//flag marking whether the TX is in the cycle of observing the channel
	bool is_channel_observed_;
	//timestamp when packet was loaded into sending buffer
	size_t transmit_timestamp_;
protected:
	
};
#endif //TRANSMITTER_H_
