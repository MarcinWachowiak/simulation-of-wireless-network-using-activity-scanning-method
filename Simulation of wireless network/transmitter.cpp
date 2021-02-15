#include "transmitter.h"
#include "simulation.h"

//Class representing a transmitting unit
//It is responsible for storing packets ready to send
//observing the channel and counting it's clean time,
//starting transmission/retransmission removing packet
//and maintaining statistics

Transmitter::Transmitter(int tx_id, int max_retran_count, int kTxInitDownCounterVal, Receiver* receiver)
	: kTxId_(tx_id),
	kMaxRetranCount_(max_retran_count),
	kTxInitDownCounterVal_(kTxInitDownCounterVal),
	transmission_time_(-1),
	curr_cln_time_down_counter_(kTxInitDownCounterVal),
	curr_retran_num_(0),
	wait_time_multpl_(0),
	was_tran_ok_(false),
	is_busy_(false),
	coupled_receiver_(receiver),
	was_channel_empty_(false),
	  is_listening_finished_(false),
	  is_active_(true),
	  is_probing_active_(false),
	  is_channel_observed_(false),
	  transmit_timestamp_(0)
{
}

Transmitter::~Transmitter()
{
	while (!buffer_.empty())
	{
		Packet* tmp = buffer_.front();
		buffer_.pop();
		delete tmp;
	}
}

// restore to defaults
void Transmitter::reset()
{
	transmission_time_ = -1;
	curr_cln_time_down_counter_ = kTxInitDownCounterVal_;
	curr_retran_num_ = 0;
	wait_time_multpl_ = 0;
	was_tran_ok_ = false;
	is_busy_ = false;
	was_channel_empty_ = false;
	is_listening_finished_ = false;
	is_active_ = true;
	is_probing_active_ = false;
	is_channel_observed_ = false;
	transmit_timestamp_ = 0;
	//clean the queue

	while(!buffer_.empty())
	{
		Packet* tmp = buffer_.front();
		buffer_.pop();
		delete tmp;
	}
}

//setters and getters
bool Transmitter::is_busy() const
{
	return is_busy_;
}

void Transmitter::set_is_busy(bool is_busy)
{
	is_busy_ = is_busy;
}

int Transmitter::get_k_tx_id() const
{
	return kTxId_;
}

int Transmitter::get_k_max_retran_count() const
{
	return kMaxRetranCount_;
}

int Transmitter::get_k_tx_init_down_counter_val() const
{
	return kTxInitDownCounterVal_;
}

std::queue<Packet*>& Transmitter::get_buffer()
{
	return buffer_;
}

int Transmitter::get_transmission_time() const
{
	return transmission_time_;
}

void Transmitter::set_transmission_time(int transmission_time)
{
	transmission_time_ = transmission_time;
}


int& Transmitter::get_curr_retran_num()
{
	return curr_retran_num_;
}

void Transmitter::set_curr_retran_num(int curr_retran_num)
{
	curr_retran_num_ = curr_retran_num;
}

int Transmitter::get_wait_time_multpl() const
{
	return wait_time_multpl_;
}

void Transmitter::set_wait_time_multpl(int wait_time_multpl)
{
	wait_time_multpl_ = wait_time_multpl;
}

bool Transmitter::was_tran_ok() const
{
	return was_tran_ok_;
}

void Transmitter::set_was_tran_ok(bool was_tran_ok)
{
	was_tran_ok_ = was_tran_ok;
}

Receiver*& Transmitter::get_coupled_receiver()
{
	return coupled_receiver_;
}

void Transmitter::set_coupled_receiver(Receiver* coupled_receiver)
{
	coupled_receiver_ = coupled_receiver;
}

bool& Transmitter::was_channel_empty()
{
	return was_channel_empty_;
}

void Transmitter::set_was_channel_empty(bool was_channel_empty)
{
	this->was_channel_empty_ = was_channel_empty;
}

bool& Transmitter::is_listening_finished()
{
	return is_listening_finished_;
}

void Transmitter::set_is_listening_finished(bool has_listening_finished)
{
	this->is_listening_finished_ = has_listening_finished;
}

bool Transmitter::is_active() const
{
	return is_active_;
}

void Transmitter::set_is_active(bool is_active)
{
	this->is_active_ = is_active;
}

bool Transmitter::is_probing_active() const
{
	return is_probing_active_;
}

void Transmitter::set_is_probing_active(bool is_probing_active)
{
	is_probing_active_ = is_probing_active;
}

bool Transmitter::is_channel_observed() const
{
	return is_channel_observed_;
}

void Transmitter::set_is_channel_observed(bool is_channel_observed)
{
	is_channel_observed_ = is_channel_observed;
}

int& Transmitter::get_curr_cln_time_down_counter()
{
	return curr_cln_time_down_counter_;
}

void Transmitter::set_curr_cln_time_down_counter(int curr_cln_time_down_counter)
{
	curr_cln_time_down_counter_ = curr_cln_time_down_counter;
}

size_t& Transmitter::get_transmit_timestamp()
{
	return transmit_timestamp_;
}

void Transmitter::set_transmit_timestamp(size_t transmit_timestamp)
{
	transmit_timestamp_ = transmit_timestamp;
}