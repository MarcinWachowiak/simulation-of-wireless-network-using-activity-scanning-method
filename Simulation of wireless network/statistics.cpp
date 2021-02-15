#include "simulation.h"
#include "statistics.h"
#include <algorithm>
#include <numeric>

//Class used to gather and update system parameters during simulation
//Each object has a pointer to this class in case of an event
//it automatically updates the counters (performs any necessary calculations)

Statistics::Statistics()
	: tot_retran_count_(0),
	tot_packet_await_time_(0),
	tot_packet_delay_(0),
	simulation_ptr_(nullptr)
{
}

Statistics::~Statistics()
{
	successfully_received_packets_vector_.erase(successfully_received_packets_vector_.begin(), successfully_received_packets_vector_.end());
	lost_packets_vector_.erase(lost_packets_vector_.begin(), lost_packets_vector_.end());
}
//reset all to default
void Statistics::reset()
{
	tot_retran_count_ = 0;
	tot_packet_await_time_ = 0;
	tot_packet_delay_ = 0;

	std::fill(successfully_received_packets_vector_.begin(), successfully_received_packets_vector_.end(), 0);
	std::fill(lost_packets_vector_.begin(), lost_packets_vector_.end(), 0);
}
// get sum of all properly received packets
float Statistics::getTotalReceivedPacketsCount()
{
	return (static_cast<float>(std::accumulate(successfully_received_packets_vector_.begin(), successfully_received_packets_vector_.end(), 0)));
}
//get sum of all lost 
float Statistics::getTotalLostPacketsCount()
{
	return (static_cast<float>(std::accumulate(lost_packets_vector_.begin(), lost_packets_vector_.end(), 0)));
}
//get a vector of calculated PER per each RX
std::vector<float> Statistics::getPacketErrorRatePerRX()
{
	std::vector<float> packet_error_rate_per_rx;

	for(size_t i =0; i<lost_packets_vector_.size();i++)
	{
		float lost_packet_count = static_cast<float> (lost_packets_vector_.at(i));
		float received_packet_count = static_cast<float>(successfully_received_packets_vector_.at(i));
		if (received_packet_count != 0)
		{
			packet_error_rate_per_rx.push_back(lost_packet_count / received_packet_count);
		}
		else
		{
			packet_error_rate_per_rx.push_back(0.0);
		}
	}
	return packet_error_rate_per_rx;
}
//get maximum of all PERs in vector 
float Statistics::getMaxPacketErrorRate()
{
	std::vector<float> packet_error_rate_per_rx;

	for (size_t i = 0; i < lost_packets_vector_.size(); i++)
	{
		float received_packet_count = static_cast<float> (successfully_received_packets_vector_.at(i));
		float lost_packets_count = static_cast<float> (lost_packets_vector_.at(i));

		if(received_packet_count != 0)
		{
			packet_error_rate_per_rx.push_back(lost_packets_count / received_packet_count);
		}
		else
		{
			packet_error_rate_per_rx.push_back(0.0);
		}
	}
	return *max_element(packet_error_rate_per_rx.begin(), packet_error_rate_per_rx.end());
}
//get averaged PER of the whole system
float Statistics::getAvgPacketErrorRate()
{
	float total_lost_packets = static_cast<float>(std::accumulate(lost_packets_vector_.begin(), lost_packets_vector_.end(), 0));
	float total_received_packet_count = getTotalReceivedPacketsCount();
	if (total_received_packet_count == 0)
	{
		return 0.0;
	}
	else
	{
		return total_lost_packets / total_received_packet_count;
	}
}
//get average retransmission count per single packet
float Statistics::getAvgRetransmissionCount()
{
	float total_received_packet_count = getTotalReceivedPacketsCount();
	if (total_received_packet_count == 0)
	{
		return 0.0;
	}
	else
	{
		return static_cast<float>(tot_retran_count_) / total_received_packet_count;
	}
}
//get all received packets divided by clock value
float Statistics::getTotalSystemThroughputPerSimQuant ()
{
	float total_received_packets = getTotalReceivedPacketsCount();
	float time_elapsed = static_cast<float>(simulation_ptr_->get_sim_clock()-simulation_ptr_->get_minimal_time_step());
	
	return total_received_packets / time_elapsed;
}
//calculate average packet delay time in system (time the packets waits to be received properly)
float Statistics::getAvgPacketDelayTime()
{
	float total_received_packet_count = getTotalReceivedPacketsCount();
	if (total_received_packet_count == 0)
	{
		return 0.0;
	}else
	{
		return static_cast<float>(tot_packet_delay_) / total_received_packet_count;

	}
}
//calculate average packet await time in system (time that packet spends waiting for proper reception since starting first transmission)
float Statistics::getAvgPacketAwaitTime()
{
	float total_received_packet_count = getTotalReceivedPacketsCount();
	if (total_received_packet_count == 0)
	{
		return 0.0;
	}
	else
	{
		return static_cast<float>(tot_packet_await_time_) / total_received_packet_count;

	}
}

//setters and getters
std::vector<int>& Statistics::get_successfully_received_packets_vector()
{
	return successfully_received_packets_vector_;
}

std::vector<int>& Statistics::get_lost_packets_vector()
{
	return lost_packets_vector_;
}

size_t& Statistics:: get_tot_retran_count()
{
	return tot_retran_count_;
}

void Statistics::set_tot_retran_count(size_t tot_avg_retran_count)
{
	tot_retran_count_ = tot_avg_retran_count;
}

size_t& Statistics::get_tot_packet_await_time()
{
	return tot_packet_await_time_;
}

void Statistics::set_tot_packet_await_time(size_t tot_avg_packet_await_time)
{
	tot_packet_await_time_ = tot_avg_packet_await_time;
}

size_t& Statistics::get_tot_packet_delay()
{
	return tot_packet_delay_;
}

void Statistics::set_tot_packet_delay(size_t tot_avg_packet_delay)
{
	tot_packet_delay_ = tot_avg_packet_delay;
}

void Statistics::set_simulation_ptr(Simulation* simulation_ptr)
{
	simulation_ptr_ = simulation_ptr;
}


