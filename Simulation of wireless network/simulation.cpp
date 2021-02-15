#include <conio.h>
#include <iostream>
#include "simulation.h"
#include "generate_packet_event.h"
#include "mark_empty_channel_state_event.h"
#include "start_transmission_event.h"
#include "transmit_ack_event.h"
#include "successful_packet_transmission_event.h"
#include "remove_lost_packet_event.h"
#include "schedule_retransmission_event.h"
#include "end_tx_sleep_event.h"
#include "end_ack_transmission_event.h"
#include "reset_tx_down_counter_event.h"
#include "channel_busy_event.h"
#include "channel_empty_event.h"
#include "collision_event.h"
#include "start_channel_observing_event.h"
#include "probe_channel_state_event.h"
#include "reduce_tx_down_counter_event.h"
#include "update_channel_state_event.h"
#include "start_retransmission_event.h"

//master class containing every element present in the simulation
//used to implement proper simulation technique actions, global clock
//organizes and supervises activity review cycles

Simulation::Simulation(size_t kInitialPhaseTimeLength, size_t kTargetSimulationTime, int minimal_time_step, int channel_check_interval, double kPacketRngIntensity, double kChannelProbabilityOfSuccess, WirelessNetwork* wireless_network, spdlog::logger* logger)
	:
	kInitialPhaseTimeLength_(kInitialPhaseTimeLength),
	kPacketRngIntensity_(kPacketRngIntensity),
	kChannelProbabilityOfSuccess_(kChannelProbabilityOfSuccess),
	minimal_time_step_(minimal_time_step),
	channel_check_interval_(channel_check_interval),
	was_event_triggered_(false),
	target_sim_time_(kTargetSimulationTime),
	clock_(0),
	wireless_network_(wireless_network),
	logger_(logger),
	target_avg_packet_error_rate_(-1),
	target_avg_retransmission_count_(-1),
	target_total_received_packets_count_(-1),
	target_total_lost_packets_count_(-1),
	simulation_run_index_(0),
	save_initial_phase_data_(false),
	init_phase_data_filename_("")
{
	//link events to objects, events to events etc.
	//attach simulation to the wireless network
	
	int loc_rx_tx_count = wireless_network_->get_k_rx_tx_count();

	
	//channel watchers
	auto  update_channel_state_event = new UpdateChannelStateEvent(-1, this, wireless_network->get_channel());
	update_channel_state_event_ = update_channel_state_event;
	
	auto* channel_empty_event = new ChannelEmptyEvent(-1, this, wireless_network_->get_channel());
	channel_empty_event_ = channel_empty_event;

	auto* channel_busy_event = new ChannelBusyEvent(-1, this, wireless_network->get_channel());
	channel_busy_event_ = channel_busy_event;

	auto* collision_event = new CollisionEvent(-1, this, wireless_network_->get_channel());
	collision_event_ = collision_event;

	//single rng
	auto* msg_corrupted_by_channel_rng = new RandomNumGen(-1);
	msg_corrupted_by_channel_rng_ = msg_corrupted_by_channel_rng;
	
	//for each RX & TX couple
	for (int i = 0; i < loc_rx_tx_count; i++)
	{
		//RNG
		//Instantiate RNG generators
		//load with default seed -1
		//seed will be replaced by loadRNGSeeds() function
		auto* packet_generation_rng = new RandomNumGen(-1);
		packet_generation_rngs_.push_back(packet_generation_rng);
		
		auto* packet_transmit_time_rng = new RandomNumGen(-1);
		packet_transmit_time_rngs_.push_back(packet_transmit_time_rng);
		
		auto* sleep_time_rng = new RandomNumGen(-1);
		sleep_time_rngs_.push_back(sleep_time_rng);
		
		//EVENTS
		//packet gen
		auto* packet_gen_event = new GeneratePacketEvent(-1, this, wireless_network_->get_transmitters().at(i), packet_generation_rng);
		generate_packet_events_.push_back(packet_gen_event);
		//channel sniffing and updating
		auto probe_channel_state_event = new ProbeChannelStateEvent(-1, this, wireless_network_->get_transmitters().at(i));
		probe_channel_state_events_.push_back(probe_channel_state_event);

		auto* start_channel_observing_event = new StartChannelObservingEvent(-1,this, wireless_network_->get_transmitters().at(i),probe_channel_state_event);
		start_channel_observing_events_.push_back(start_channel_observing_event);

		auto* mark_empty_channel_state_event = new MarkEmptyChannelStateEvent(-1,this, wireless_network_->get_transmitters().at(i));
		mark_empty_channel_state_events_.push_back(mark_empty_channel_state_event);

		auto* reset_tx_down_counter_event = new ResetTXDownCounterEvent(-1, this, wireless_network_->get_transmitters().at(i));
		reset_tx_down_counter_events_.push_back(reset_tx_down_counter_event);
		
		auto* reduce_tx_down_counter_event = new ReduceTXDownCounterEvent(-1, this, wireless_network_->get_transmitters().at(i));
		reduce_tx_down_counter_events_.push_back(reduce_tx_down_counter_event);

		//end transmission events
		auto* end_ack_listening_event = new EndAckListeningEvent(-1, this, wireless_network_->get_transmitters().at(i));
		end_ack_listening_events_.push_back(end_ack_listening_event);
		
		auto* end_packet_transmission_event = new EndPacketTransmissionEvent(-1, this, nullptr,msg_corrupted_by_channel_rng);
		end_packet_transmission_events_.push_back(end_packet_transmission_event);

		auto* end_ack_transmission_event = new EndAckTransmissionEvent(-1, this, nullptr);
		end_ack_transmission_events_.push_back(end_ack_transmission_event);
		
		//starting transmissions
		auto* start_transmission_event = new StartTransmissionEvent(-1,this, wireless_network_->get_transmitters().at(i),packet_transmit_time_rng,end_ack_listening_event,end_packet_transmission_event,probe_channel_state_event);
		start_transmission_events_.push_back(start_transmission_event);
		//retrans
		auto* start_retransmission_event = new StartRetransmissionEvent(-1, this, wireless_network_->get_transmitters().at(i), packet_transmit_time_rng, end_ack_listening_event, end_packet_transmission_event, probe_channel_state_event);
		start_retransmission_events_.push_back(start_retransmission_event);

		//after transmission procedures
		auto*  successful_packet_transmission_event = new SuccessfulPacketTransmissionEvent(-1, this, wireless_network_->get_transmitters().at(i), end_ack_listening_event);
		successful_packet_transmission_events_.push_back(successful_packet_transmission_event);
		
		auto* remove_lost_packet_event = new RemoveLostPacketEvent(-1, this, wireless_network_->get_transmitters().at(i));
		remove_lost_packet_events_.push_back(remove_lost_packet_event);
		
		auto* end_tx_sleep_event = new EndTxSleepEvent(-1, this, wireless_network_->get_transmitters().at(i));
		end_tx_sleep_events_.push_back(end_tx_sleep_event);
		
		auto* schedule_retransmission_event = new ScheduleRetransmissionEvent(-1, this, wireless_network_->get_transmitters().at(i), sleep_time_rng, end_tx_sleep_event,probe_channel_state_event);
		schedule_retransmission_events_.push_back(schedule_retransmission_event);

		//conditional transmit ack event (blocking initial values => will be replaced when triggered)
		auto* transmit_ack_event = new TransmitAckEvent(-1, this, wireless_network_->get_receivers().at(i), end_ack_transmission_event);
		transmit_ack_events_.push_back(transmit_ack_event);

		//link statistics to RX
		wireless_network_->get_statistics()->get_lost_packets_vector().push_back(0);
		wireless_network_->get_statistics()->get_successfully_received_packets_vector().push_back(0);
	}
}

Simulation::~Simulation()
{	
	delete wireless_network_;
	//delete rngs in vectors
	packet_generation_rngs_.erase(packet_generation_rngs_.begin(),packet_generation_rngs_.end());
	packet_transmit_time_rngs_.erase(packet_transmit_time_rngs_.begin(),packet_transmit_time_rngs_.end());
	sleep_time_rngs_.erase(sleep_time_rngs_.begin(),sleep_time_rngs_.end());

	//delete rng instances
	for (auto packet_generation_rng: packet_generation_rngs_)
	{
		delete packet_generation_rng;
	}
	for (auto packet_transmit_time_rng : packet_transmit_time_rngs_)
	{
		delete packet_transmit_time_rng;
	}
	for (auto sleep_time_rng : sleep_time_rngs_)
	{
		delete sleep_time_rng;
	}

	
	delete msg_corrupted_by_channel_rng_;

	//delete all events
	delete update_channel_state_event_;
	delete channel_empty_event_;
	delete channel_busy_event_;
	delete collision_event_;

	for(auto generate_packet_event : generate_packet_events_)
	{
		delete generate_packet_event;
	}
	for (auto probe_channel_state_event : probe_channel_state_events_)
	{
		delete probe_channel_state_event;
	}
	for (auto start_channel_observing_event : start_channel_observing_events_)
	{
		delete start_channel_observing_event;
	}
	for (auto mark_empty_channel_state_event : mark_empty_channel_state_events_)
	{
		delete mark_empty_channel_state_event;
	}
	for (auto reset_tx_down_counter_event : reset_tx_down_counter_events_)
	{
		delete reset_tx_down_counter_event;
	}
	//for (auto reduce_tx_down_counter_event : generate_packet_events_)
	//{
	//	delete reduce_tx_down_counter_event;
	//}
	for (auto end_ack_listening_event : end_ack_listening_events_)
	{
		delete end_ack_listening_event;
	}
	for (auto end_packet_transmission_event : end_packet_transmission_events_)
	{
		delete end_packet_transmission_event;
	}
	for (auto end_ack_transmission_event : end_ack_transmission_events_)
	{
		delete end_ack_transmission_event;
	}
	for (auto start_transmission_event : start_transmission_events_)
	{
		delete start_transmission_event;
	}
	for (auto start_retransmission_event : start_retransmission_events_)
	{
		delete start_retransmission_event;
	}
	for (auto successful_packet_transmission_event : successful_packet_transmission_events_)
	{
		delete successful_packet_transmission_event;
	}
	for (auto remove_lost_packet_event : remove_lost_packet_events_)
	{
		delete remove_lost_packet_event;
	}
	for (auto end_tx_sleep_event : end_tx_sleep_events_)
	{
		delete end_tx_sleep_event;
	}
	for (auto schedule_retransmission_event : schedule_retransmission_events_)
	{
		delete schedule_retransmission_event;
	}
	for (auto transmit_ack_event : transmit_ack_events_)
	{
		delete transmit_ack_event;
	}
	
	
	generate_packet_events_.erase(generate_packet_events_.begin(), generate_packet_events_.end());
	probe_channel_state_events_.erase(probe_channel_state_events_.begin(), probe_channel_state_events_.end());
	start_channel_observing_events_.erase(start_channel_observing_events_.begin(), start_channel_observing_events_.end());
	mark_empty_channel_state_events_.erase(mark_empty_channel_state_events_.begin(), mark_empty_channel_state_events_.end());
	reset_tx_down_counter_events_.erase(reset_tx_down_counter_events_.begin(), reset_tx_down_counter_events_.end());
	reduce_tx_down_counter_events_.erase(reduce_tx_down_counter_events_.begin(), reduce_tx_down_counter_events_.end());
	end_ack_listening_events_.erase(end_ack_listening_events_.begin(), end_ack_listening_events_.end());
	end_packet_transmission_events_.erase(end_packet_transmission_events_.begin(), end_packet_transmission_events_.end());
	end_ack_transmission_events_.erase(end_ack_transmission_events_.begin(), end_ack_transmission_events_.end());
	start_transmission_events_.erase(start_transmission_events_.begin(), start_transmission_events_.end());
	start_retransmission_events_.erase(start_retransmission_events_.begin(), start_retransmission_events_.end());
	successful_packet_transmission_events_.erase(successful_packet_transmission_events_.begin(), successful_packet_transmission_events_.end());
	remove_lost_packet_events_.erase(remove_lost_packet_events_.begin(), remove_lost_packet_events_.end());
	end_tx_sleep_events_.erase(end_tx_sleep_events_.begin(), end_tx_sleep_events_.end());
	schedule_retransmission_events_.erase(schedule_retransmission_events_.begin(), schedule_retransmission_events_.end());
	transmit_ack_events_.erase(transmit_ack_events_.begin(), transmit_ack_events_.end());
}

void Simulation::showInitSimulationInfo(bool is_stepping_enabled)
{
	logger_->info("Object oriented computer simulation of discrete event systems (M1, A6e)\n");

	if (is_stepping_enabled)
	{
		logger_->warn("SIMULATION IS EXECUTED IN STEPPING MODE PRESS ANY KEY TO ADVANCE\n\t  THERE ARE NO FURTHER \"PRESS ANY KEY TO CONTINUE\" PROMPT DIALOGS TO MAINTAIN CONSOLE CLARITY\n");
	}
	else {}

	//retrieve simulation data
	int kTxAndRxPairs = wireless_network_->get_k_rx_tx_count();
	int kMaxRetranNum = wireless_network_->get_transmitters().at(0)->get_k_max_retran_count();
	int kRequiredChannelFreeTimeDIFS = wireless_network_->get_k_tx_init_down_counter_val() - get_channel_check_interval();
	//display simulation preset
	logger_->info("Simulation details:\n");
	logger_->info("Simulation Index:\t\t\t\t{0}", simulation_run_index_);
	logger_->info("Packet random number generator intensity(L):\t{0}",kPacketRngIntensity_);
	logger_->info("Channel probability of success (TER):\t\t{0}", kChannelProbabilityOfSuccess_);
	logger_->info("Initial phase time length:\t\t\t{0}\t[ms]", kInitialPhaseTimeLength_);
	logger_->info("Simulation time:\t\t\t\t\t{0}\t[ms]",target_sim_time_);
	logger_->info("TX and RX pair count(k):\t\t\t\t{0}", kTxAndRxPairs);
	logger_->info("Maximum retransmission number(LR):\t\t{0}",kMaxRetranNum);
	logger_->info("Required channel free time (DIFS):\t\t{0}\n",kRequiredChannelFreeTimeDIFS);
	
	//display formatting
	logger_->info("Formatting:\n[level] [timestamp] Description with optional details");
}

void Simulation::showSimulationResults ()
{
	Statistics* tmp_statistics_ptr = wireless_network_->get_statistics();
	std::cerr << std::endl;
	logger_->info("Simulation finished with results:\n");
	logger_->info("Average packet error rate:\t{:0.5f}", tmp_statistics_ptr->getAvgPacketErrorRate());
	logger_->info("Maximum packet error rate:\t{:0.5f}", tmp_statistics_ptr->getMaxPacketErrorRate());
	logger_->info("Average retransmission count:\t{:0.3f}\t[per packet]", tmp_statistics_ptr->getAvgRetransmissionCount());;
	logger_->info("System throughput:\t\t{:0.2f}\t[packets/s]", (tmp_statistics_ptr->getTotalSystemThroughputPerSimQuant() * 10000));;
	logger_->info("Average packet delay time:\t{:0.2f}\t[ms]", (tmp_statistics_ptr->getAvgPacketDelayTime() / 10));;
	logger_->info("Average packet await time:\t{:0.2f}\t[ms]\n", (tmp_statistics_ptr->getAvgPacketAwaitTime() / 10));;
	logger_->info("Packet error rate per each RX:");
	for (Receiver* receiver : wireless_network_->get_receivers())
	{
		logger_->info("RX id: {:0} PER: {:1.5f}", receiver->get_k_rx_id(), tmp_statistics_ptr->getPacketErrorRatePerRX().at(receiver->get_k_rx_id()));;
	}
}

//Insert new seeds into RNG generators present in the simulation model
void Simulation:: loadRNGSeeds(std::vector<int> seed_set)
{
	//mismatch check
	if(packet_transmit_time_rngs_.size() + sleep_time_rngs_.size() + packet_generation_rngs_.size() + 1 != seed_set.size())
	{
		logger_->error("Number of seeds provided in vector/file does not match seed count required by simulation");
	}

	int i = 0;
		
	for (RandomNumGen* packet_generation_rng : packet_generation_rngs_)
	{
		packet_generation_rng->set_k_kernel(seed_set.at(i));
		++i;
	}

	for (RandomNumGen* packet_transmit_time_rng : packet_transmit_time_rngs_)
	{
		packet_transmit_time_rng->set_k_kernel(seed_set.at(i));
		++i;
	}

	for (RandomNumGen* sleep_time_rng : sleep_time_rngs_)
	{
		sleep_time_rng->set_k_kernel(seed_set.at(i));
		++i;
	}

	msg_corrupted_by_channel_rng_->set_k_kernel(seed_set.at(i));
}

void Simulation::resetAndInitialize()
{
	//reset any old result variables
	clock_ = 0;
	was_event_triggered_ = false;

	//reset time events variables
	for (size_t i = 0; i < generate_packet_events_.size(); ++i)
	{
		generate_packet_events_.at(i)->set_time(packet_generation_rngs_.at(i)->randExpRoundedInt(kPacketRngIntensity_));
	}
	update_channel_state_event_->set_time(0);

	//reset all timed events
	for (Event* end_packet_transmission_event : end_packet_transmission_events_)
	{
			end_packet_transmission_event->set_time(-1);
	}
	for (Event* end_ack_transmission_event : end_ack_transmission_events_)
	{
			end_ack_transmission_event->set_time(-1);
	}
	for (Event* end_ack_listening_event : end_ack_listening_events_)
	{
			end_ack_listening_event->set_time(-1);
	}
	for (Event* end_tx_sleep_event : end_tx_sleep_events_)
	{
			end_tx_sleep_event->set_time(-1);
	}
	for (Event* probe_channel_state : probe_channel_state_events_)
	{
			probe_channel_state->set_time(-1);
	}

	wireless_network_->reset();
	wireless_network_->get_statistics()->reset();
}

void Simulation::set_target_avg_packet_error_rate(float target_avg_packet_error_rate)
{
	target_avg_packet_error_rate_ = target_avg_packet_error_rate;
}

void Simulation::set_target_avg_retransmission_count(float target_avg_retransmission_count)
{
	target_avg_retransmission_count_ = target_avg_retransmission_count;
}

void Simulation::set_target_total_packets_received_count(int target_total_packets_received_count)
{
	target_total_received_packets_count_ = target_total_packets_received_count;
}

void Simulation::set_target_total_lost_packets_count(int target_total_lost_packets_count)
{
	target_total_lost_packets_count_ = target_total_lost_packets_count;
}

//main loop is quite long may be split into two methods:
//activity scanning and clock update to reduce line count
void Simulation::run(bool is_stepping_enabled )
{

	//optional reset state and variables
	//resetAndInitialize();
	showInitSimulationInfo(is_stepping_enabled);

	bool was_info_shown;
	while(clock_<=target_sim_time_)
	{
		
		if(isInitialPhaseOver() && isTargetCriteriaMet())
		{
			std::cerr << std::endl;
			logger_->warn("***Simulation terminated earlier due to reaching specified target values***");
			break;
		}

		//flag used not to duplicate timestamp
		//log information display to maintain transparency
		was_info_shown = false;
		was_event_triggered_ = true;
		//main sim loop
		while(was_event_triggered_==true)
		{
			was_event_triggered_ = false;
			//all timed events:
			
			for (Event* generate_packet_event : generate_packet_events_)
			{
				if (generate_packet_event->get_time() == 0)
				{
					generate_packet_event->execute();
					was_event_triggered_ = true;
					was_info_shown = true;
				}
			}
			//
			if (update_channel_state_event_->get_time() == 0)
			{
				update_channel_state_event_->execute();
				was_event_triggered_ = true;
				was_info_shown = true;
			}
			//conditionals
			//update channel state:
			if (channel_empty_event_->isTriggered())
			{
				channel_empty_event_->execute();
				was_event_triggered_ = true;
			}

			if (channel_busy_event_->isTriggered())
			{
				channel_busy_event_->execute();
				was_event_triggered_ = true;
			}

			if (collision_event_->isTriggered())
			{
				collision_event_->execute();
				was_event_triggered_ = true;
			}
			//END
			for (Event* end_packet_transmission_event : end_packet_transmission_events_)
			{
				if (end_packet_transmission_event->get_time() == 0)
				{
					end_packet_transmission_event->execute();
					was_event_triggered_ = true;
					was_info_shown = true;
				}
			}
			for (Event* end_ack_transmission_event : end_ack_transmission_events_)
			{
				if (end_ack_transmission_event->get_time() == 0)
				{
					end_ack_transmission_event->execute();
					was_event_triggered_ = true;
					was_info_shown = true;
				}
			}
			for (Event* end_ack_listening_event : end_ack_listening_events_)
			{
				if (end_ack_listening_event->get_time() == 0)
				{
					end_ack_listening_event->execute();
					was_event_triggered_ = true;
					was_info_shown = true;
				}
			}
			for (Event* end_tx_sleep_event : end_tx_sleep_events_)
			{
				if (end_tx_sleep_event->get_time() == 0)
				{
					end_tx_sleep_event->execute();
					was_event_triggered_ = true;
					was_info_shown = true;
				}
			}
			//check tran results
			for (Event* successful_packet_transmission_event : successful_packet_transmission_events_)
			{
				if (successful_packet_transmission_event->isTriggered())
				{
					successful_packet_transmission_event->execute();
					was_event_triggered_ = true;
					was_info_shown = true;

				}
			}
			for (Event* remove_lost_packet_event : remove_lost_packet_events_)
			{
				if (remove_lost_packet_event->isTriggered())
				{
					remove_lost_packet_event->execute();
					was_event_triggered_ = true;
					was_info_shown = true;

				}
			}
			for (Event* schedule_retransmission_event : schedule_retransmission_events_)
			{
				if (schedule_retransmission_event->isTriggered())
				{
					schedule_retransmission_event->execute();
					was_event_triggered_ = true;
					was_info_shown = true;

				}
			}
			for (Event* start_channel_observing_event : start_channel_observing_events_)
			{
				if (start_channel_observing_event->isTriggered())
				{
					start_channel_observing_event->execute();
					was_event_triggered_ = true;
					was_info_shown = true;

				}
			}
			for (Event* probe_channel_state : probe_channel_state_events_)
			{
				if (probe_channel_state->get_time() == 0)
				{
					probe_channel_state->execute();
					was_event_triggered_ = true;
					was_info_shown = true;
				}
			}
			//chan observing
			
			for (Event* mark_empty_channel_state_event : mark_empty_channel_state_events_)
			{
				if (mark_empty_channel_state_event->isTriggered())
				{
					mark_empty_channel_state_event->execute();
					was_event_triggered_ = true;
					was_info_shown = true;

				}
			}
			for (Event* reset_tx_down_counter_event : reset_tx_down_counter_events_)
			{
				if (reset_tx_down_counter_event->isTriggered())
				{
					reset_tx_down_counter_event->execute();
					was_event_triggered_ = true;
					was_info_shown = true;

				}
			}
			for (Event* reduce_tx_down_counter_event : reduce_tx_down_counter_events_)
			{
				if (reduce_tx_down_counter_event->isTriggered())
				{
					reduce_tx_down_counter_event->execute();
					was_event_triggered_ = true;
					was_info_shown = true;

				}
			}
			//start trans
			for (Event* transmit_ack_event : transmit_ack_events_)
			{
				if (transmit_ack_event->isTriggered())
				{
					transmit_ack_event->execute();
					was_event_triggered_ = true;
					was_info_shown = true;

				}
			}
			for (Event* start_retransmission_event : start_retransmission_events_)
			{
				if (start_retransmission_event->isTriggered())
				{
					start_retransmission_event->execute();
					was_event_triggered_ = true;
					was_info_shown = true;
				}
			}
			for(Event* start_transmission_event :start_transmission_events_)
			{
				if(start_transmission_event->isTriggered())
				{
					start_transmission_event->execute();
					was_event_triggered_ = true;
					was_info_shown = true;
				}
			}
		}
		
		//update clocks of active events
		if (update_channel_state_event_->isActive())
		{
			update_channel_state_event_->get_time() -= minimal_time_step_;
		}
		else {}
		for (Event* generate_packet_event : generate_packet_events_)
		{
			if(generate_packet_event->isActive())
			{
				generate_packet_event->get_time() -= minimal_time_step_;
			} else {}
		}
		for (Event* end_ack_listening_event : end_ack_listening_events_)
		{
			if (end_ack_listening_event->isActive())
			{
				end_ack_listening_event->get_time() -= minimal_time_step_;
			}
			else {}
		}
		for (Event* end_ack_transmission_event : end_ack_transmission_events_)
		{
			if (end_ack_transmission_event->isActive())
			{
				end_ack_transmission_event->get_time() -= minimal_time_step_;
			}
			else {}
		}
		for (Event* end_packet_transmission_event : end_packet_transmission_events_)
		{
			if (end_packet_transmission_event->isActive())
			{
				end_packet_transmission_event->get_time() -= minimal_time_step_;
			}
			else {}
		}
		for (Event* end_tx_sleep_event : end_tx_sleep_events_)
		{
			if (end_tx_sleep_event->isActive())
			{
				end_tx_sleep_event->get_time() -= minimal_time_step_;
			}
			else {}
		}
		for (Event* probe_channel_state_event : probe_channel_state_events_)
		{
			if (probe_channel_state_event->isActive())
			{
				probe_channel_state_event->get_time() -= minimal_time_step_;
			}
			else {}
		}
		//stepping mode
		if (is_stepping_enabled)
		{
			if (!was_info_shown) {
				logger_->info("\t[{0}]\t", clock_);
			}
			//pause, get any key (used because it does not start
			//a new line or display any useless info)
			_getch();
		}
		clock_ += minimal_time_step_;
	}
	showSimulationResults();
}

//simple checker function to verify if
//initial phase has finished and statistics can be saved
bool Simulation::isInitialPhaseOver()
{
	if(clock_>kInitialPhaseTimeLength_)
	{
		return true;
	} 
	return false;
}

bool Simulation::isTargetCriteriaMet()
{
	Statistics* tmp_statistics_ptr = wireless_network_->get_statistics();
	
	if (target_avg_packet_error_rate_ > 0)
	{
		if (tmp_statistics_ptr->getAvgPacketErrorRate() >= target_avg_packet_error_rate_)
		{
			return true;
		}
	}

	if (target_avg_retransmission_count_ > 0)
	{
		if (tmp_statistics_ptr->getAvgRetransmissionCount() >= target_avg_retransmission_count_)
		{
			return true;
		}
	}

	if (target_total_received_packets_count_ > 0)
	{
		if (static_cast<int>(tmp_statistics_ptr->getTotalReceivedPacketsCount()) >= target_total_received_packets_count_)
		{
			return true;
		}
	}
	
	if (target_total_lost_packets_count_ > 0)
	{
		if (static_cast<int>(tmp_statistics_ptr->getTotalLostPacketsCount()) >= target_total_lost_packets_count_)
		{
			return true;
		}
	}
	return false;
}


//setters and getters
int Simulation::get_minimal_time_step() const
{
	return minimal_time_step_;
}


spdlog::logger*& Simulation::get_logger()
{
	return logger_;
}

bool Simulation::was_event_triggered() const
{
	return was_event_triggered_;
}

void Simulation::set_was_event_triggered(bool is_event_triggered)
{
	was_event_triggered_ = is_event_triggered;
}

size_t Simulation::get_sim_clock() const
{
	return clock_;
}

void Simulation::set_sim_clock(size_t sim_time)
{
	clock_ = sim_time;
}

WirelessNetwork* Simulation::get_wireless_network() const
{
	return wireless_network_;
}

void Simulation::set_wireless_network(WirelessNetwork* wireless_network)
{
	wireless_network_ = wireless_network;
}

int& Simulation::get_channel_check_interval()
{
	return channel_check_interval_;
}

void Simulation::set_channel_check_interval(int channel_check_interval)
{
	channel_check_interval_ = channel_check_interval;
}

std::vector<RandomNumGen*>& Simulation::get_packet_generation_rngs()
{
	return packet_generation_rngs_;
}

std::vector<RandomNumGen*>& Simulation::get_packet_transmit_time_rngs()
{
	return packet_transmit_time_rngs_;
}

std::vector<RandomNumGen*>& Simulation::get_sleep_time_rngs()
{
	return sleep_time_rngs_;
}

RandomNumGen*& Simulation::get_msg_corrupted_by_channel_rng()
{
	return msg_corrupted_by_channel_rng_;
}

size_t Simulation::get_k_initial_phase_time_length() const
{
	return kInitialPhaseTimeLength_;
}

double& Simulation::get_k_packet_rng_intensity()
{
	return kPacketRngIntensity_;
}


double& Simulation::get_k_channel_probability_of_success()
{
	return kChannelProbabilityOfSuccess_;
}

void Simulation::set_k_packet_rng_intensity(double packet_rng_intensity)
{
	kPacketRngIntensity_ = packet_rng_intensity;
}

void Simulation::set_k_channel_probability_of_success(double channel_probability_of_success)
{
	kChannelProbabilityOfSuccess_ = channel_probability_of_success;
}

int& Simulation::get_simulation_run_index()
{
	return simulation_run_index_;
}

void Simulation::set_simulation_run_index(int simulation_run_index)
{
	simulation_run_index_ = simulation_run_index;
}

bool& Simulation::is_save_initial_phase_data()
{
	return save_initial_phase_data_;
}

void Simulation::set_save_initial_phase_data(bool save_initial_phase_data)
{
	save_initial_phase_data_ = save_initial_phase_data;
}

std::string& Simulation::get_init_phase_data_filename()
{
	return init_phase_data_filename_;
}

void Simulation::set_init_phase_data_filename(std::string init_phase_data_filename)
{
	init_phase_data_filename_ = init_phase_data_filename;
}

size_t& Simulation::get_target_sim_time()
{
	return target_sim_time_;
}


