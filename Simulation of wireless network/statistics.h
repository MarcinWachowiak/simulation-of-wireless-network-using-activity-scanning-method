#ifndef STATISTICS_H_
#define STATISTICS_H_

class Simulation;
//Class used to gather and update system parameters during simulation
//Each object has a pointer to this class in case of an event
//it automatically updates the counters (performs any neccessary calculations)

class Statistics
{
public:
	Statistics();
	~Statistics();

	void reset();
	std::vector<float> getPacketErrorRatePerRX();
	float getTotalReceivedPacketsCount();
	float getTotalLostPacketsCount();
	float getMaxPacketErrorRate();
	float getAvgPacketErrorRate();
	float getAvgRetransmissionCount();
	float getTotalSystemThroughputPerSimQuant();
	float getAvgPacketDelayTime();
	float getAvgPacketAwaitTime();

	//setters and getters

	bool is_terminator_set() const;
	std::vector<int>& get_successfully_received_packets_vector();
	std::vector<int>& get_lost_packets_vector();
	size_t& get_tot_retran_count();
	void set_tot_retran_count(size_t tot_avg_retran_count);
	size_t& get_tot_packet_await_time();
	void set_tot_packet_await_time(size_t tot_avg_packet_await_time);
	size_t& get_tot_packet_delay();
	void set_tot_packet_delay(size_t tot_avg_packet_delay);
	void set_simulation_ptr(Simulation* simulation_ptr);
private:

	//container representing packet error received_packets/lost_packets rate
	//in each transmitter - receiver instance
	//(coupled values: receiver_id, PER)
	std::vector<int> successfully_received_packets_vector_;
	std::vector<int> lost_packets_vector_;
	//total average retransmission counter
	size_t tot_retran_count_;
	//averaged time that passes between specified packet generation and its successful reception
	size_t tot_packet_await_time_;
	//averaged time each packet waits since generation till being deleted (leaving the queue)
	size_t tot_packet_delay_;
	//ptr to simulation obj instance to retrieve clocks
	Simulation* simulation_ptr_;
protected:
	
};
#endif //STATISTICS_H_

