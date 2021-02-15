#ifndef SIMULATION_H_
#define SIMULATION_H_

#include "event.h"
#include "random_num_gen.h"
#include "wireless_network.h"
#include "spdlog/logger.h"

class TransmitAckEvent;
class StartTransmissionEvent;
class CollisionEvent;
class MarkEmptyChannelStateEvent;
class ResetTXDownCounterEvent;
class ChannelEmptyEvent;
class ChannelBusyEvent;
class SuccessfulPacketTransmissionEvent;
class StartChannelObservingEvent;
class ScheduleRetransmissionEvent;
class RemoveLostPacketEvent;
class ReduceTXDownCounterEvent;
class ProbeChannelStateEvent;
class UpdateChannelStateEvent;
class StartRetransmissionEvent;

//master class containing every element present in the simulation
//used to implement proper simulation technique actions, global clock
//organizes and supervises activity review cycles

class Simulation
{
public:
	Simulation(
		size_t kInitialPhaseTimeLength,
		size_t kTargetSimulationTime,
		int minimal_time_step, 
		int channel_check_interval,
		double kPacketRngIntensity,
		double kChannelProbabilityOfSuccess,
		WirelessNetwork* wireless_network,
	    spdlog::logger* logger);
	
	~Simulation();

	void showInitSimulationInfo(bool is_stepping_enabled);
	void showSimulationResults();
	void loadRNGSeeds(std::vector<int> seed_set);
	void run(bool is_stepping_enabled);
	bool isInitialPhaseOver();
	bool isTargetCriteriaMet();

	void resetAndInitialize();
	
	//setters and getters:
	void set_target_avg_packet_error_rate(float target_avg_packet_error_rate);
	void set_target_avg_retransmission_count(float target_avg_retransmission_count);
	void set_target_total_packets_received_count(int target_total_packets_received_count);
	void set_target_total_lost_packets_count(int target_total_lost_packets_count);
	size_t& get_target_sim_time();
	int get_minimal_time_step() const;
	spdlog::logger*& get_logger();
	bool was_event_triggered() const;
	void set_was_event_triggered(bool is_event_triggered);
	size_t get_sim_clock() const;
	void set_sim_clock(size_t sim_time);
	WirelessNetwork* get_wireless_network() const;
	void set_wireless_network(WirelessNetwork* wireless_network);
	int& get_channel_check_interval();
	void set_channel_check_interval(int channel_check_interval);
	std::vector<RandomNumGen*>& get_packet_generation_rngs();
	std::vector<RandomNumGen*>& get_packet_transmit_time_rngs();
	std::vector<RandomNumGen*>& get_sleep_time_rngs();
	RandomNumGen*& get_msg_corrupted_by_channel_rng();
	size_t get_k_initial_phase_time_length() const;
	double& get_k_packet_rng_intensity();
	double& get_k_channel_probability_of_success();
	
	//additional data
	void set_k_packet_rng_intensity(double packet_rng_intensity);
	void set_k_channel_probability_of_success(double channel_probability_of_success);
	int& get_simulation_run_index();
	void set_simulation_run_index(int simulation_run_index);
	bool& is_save_initial_phase_data();
	void set_save_initial_phase_data(bool save_initial_phase_data);
	std::string& get_init_phase_data_filename();
	void set_init_phase_data_filename(std::string init_phase_data_filename);
private:
	//time required by the simulation to stabilize
	const size_t kInitialPhaseTimeLength_;
	//exponential RNG parameter that the simulation will be evaluated over
	double kPacketRngIntensity_;
	//coefficient of proper transmission reception through channel
	double kChannelProbabilityOfSuccess_{};
	//minimal time that the simulation will advance by
	int minimal_time_step_;
	//time step equal to span between regular channel checks (5 = 0,5ms)
	int channel_check_interval_;
	//flag used as indicator to perform time or dependent actions
	bool was_event_triggered_;
	//target simulation time length
	size_t target_sim_time_;
	//global simulation timer 
	size_t clock_;
	//ptr to simulated network object
	WirelessNetwork* wireless_network_;
	//ptr to logger instance used in progress description
	spdlog::logger* logger_;
	
	//before starting the simulation, event vectors will be filled
	//with inactive events that will be modified as the simulation advances
	//whrere necessary events will be linked to their dependencies etc.
	//activity scanning will be performed in an ordered way to prevent
	//re-triggering or blocking time events

	//single conditional events
	//channel related
	CollisionEvent* collision_event_;
	ChannelBusyEvent* channel_busy_event_;
	ChannelEmptyEvent* channel_empty_event_;
	UpdateChannelStateEvent* update_channel_state_event_;
	//time based events
	std::vector<Event*> generate_packet_events_;

	//end transmission events
	std::vector<Event*> end_ack_listening_events_;
	std::vector<Event*> end_ack_transmission_events_;
	std::vector<Event*> end_packet_transmission_events_;

	//conditional events
	//starting transmission
	std::vector <StartTransmissionEvent*> start_transmission_events_;
	std::vector <StartRetransmissionEvent*> start_retransmission_events_;
	std::vector <TransmitAckEvent*> transmit_ack_events_;
	
	//after packet transmission
	std::vector <SuccessfulPacketTransmissionEvent*> successful_packet_transmission_events_;
	std::vector <RemoveLostPacketEvent*> remove_lost_packet_events_;
	std::vector <ScheduleRetransmissionEvent*> schedule_retransmission_events_;
	//timed
	std::vector<Event*> end_tx_sleep_events_;

	//chan observing
	//initial
	std::vector <StartChannelObservingEvent*> start_channel_observing_events_;
	//regular
	std::vector <ProbeChannelStateEvent*> probe_channel_state_events_;
	//conditional
	std::vector <MarkEmptyChannelStateEvent*> mark_empty_channel_state_events_;
	std::vector <ReduceTXDownCounterEvent*> reduce_tx_down_counter_events_;
	std::vector <ResetTXDownCounterEvent*> reset_tx_down_counter_events_;

	
	//PLACE FOR RNG GENERATORS
	std::vector <RandomNumGen*> packet_generation_rngs_;
	std::vector <RandomNumGen*> packet_transmit_time_rngs_;
	std::vector <RandomNumGen*> sleep_time_rngs_;
	RandomNumGen* msg_corrupted_by_channel_rng_;

	//target criteria that the simulation may be terminated by
	float target_avg_packet_error_rate_;
	float target_avg_retransmission_count_;
	int target_total_received_packets_count_;
	int target_total_lost_packets_count_;

	//additional flags
	int simulation_run_index_;
	bool save_initial_phase_data_;
	std::string init_phase_data_filename_;
	
protected:
	
};

#endif // SIMULATION_H_
