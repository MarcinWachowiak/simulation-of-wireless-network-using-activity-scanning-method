#include "simulation.h"
#include "end_ack_listening_event.h"
#include "successful_packet_transmission_event.h"
#include <sstream>
#include <iomanip>
#include <fstream>

//Event representing a conditional event of removing packet
//due to receiving ACK (successful transmission)
//packet is removed, proper statistics are saved
//and TX is reset to enable handling of any other packets

SuccessfulPacketTransmissionEvent::SuccessfulPacketTransmissionEvent(
	int time,
	Simulation* simulation,
	Transmitter* transmitter,
	EndAckListeningEvent* end_ack_listening_event) :
	Event(time,simulation),
	transmitter_(transmitter),
	end_ack_listening_event_(end_ack_listening_event)
{
}


SuccessfulPacketTransmissionEvent::~SuccessfulPacketTransmissionEvent()
{
}

void SuccessfulPacketTransmissionEvent::execute()
{
	try
	{
		//get ptr to stats to ease use
		Statistics* statistics_ptr = simulation_->get_wireless_network()->get_statistics();
		Packet* tmp_packet = transmitter_->get_buffer().front();
		if (simulation_->isInitialPhaseOver())
		{
			size_t curr_sim_time = simulation_->get_sim_clock();
			size_t generation_timestamp = tmp_packet->get_gen_timestamp();
			size_t ack_delay_time = simulation_->get_wireless_network()->get_k_ack_msg_time();

			size_t packet_await_time = transmitter_->get_transmit_timestamp() - generation_timestamp;
			size_t retran_count = transmitter_->get_curr_retran_num();
			size_t packet_delay_time = curr_sim_time - generation_timestamp - ack_delay_time;
			//add trans/packet values to global results
			statistics_ptr->get_tot_packet_await_time() += packet_await_time;
			statistics_ptr->get_tot_packet_delay() += packet_delay_time;
			//time packet spend in a queue;
			statistics_ptr->get_tot_retran_count() += retran_count;
		}
			//print newly calculated avg retran to file with timestamp when initial phase estimation is active
			try {
				if (simulation_->is_save_initial_phase_data())
				{
					std::fstream primary_file(simulation_->get_init_phase_data_filename(), std::fstream::out | std::fstream::app);
					primary_file << std::fixed << std::setprecision(3);
					primary_file << simulation_->get_sim_clock() << ";\t" << statistics_ptr->getAvgRetransmissionCount() << ";\t\n";
					primary_file.close();
				}
			}
			catch (...) {

				simulation_->get_logger()->error("[{0}]\tError while saving initial phase data log", simulation_->get_sim_clock());
			}
		
		//remove packets
		transmitter_->get_buffer().pop();
		delete tmp_packet;
		//reset temporary TX flags - reactivate the tx to normal 
		transmitter_->set_was_tran_ok(false);
		transmitter_->set_curr_cln_time_down_counter(transmitter_->get_k_tx_init_down_counter_val());
		transmitter_->set_curr_retran_num(0);
		transmitter_->set_was_channel_empty(false);
		transmitter_->set_is_listening_finished(false);
		transmitter_->set_is_active(true);
		transmitter_->set_is_probing_active(false);
		transmitter_->set_is_channel_observed(false);
		transmitter_->set_transmit_timestamp(0);
		transmitter_->set_is_busy(false);

		//deactivate listening for ack
		end_ack_listening_event_->set_time(-1);
		
		simulation_->get_logger()->debug("[{0}]\tSuccessful PACKET transmission by TX id:{1}", simulation_->get_sim_clock(), transmitter_->get_k_tx_id());
	} catch (...)
	{
		simulation_->get_logger()->error("[{0}]\tError while handling successful packet transmission event", simulation_->get_sim_clock());
	}
	
}

bool SuccessfulPacketTransmissionEvent::isTriggered()
{
	if(transmitter_->was_tran_ok() && transmitter_->is_busy())
	{
		return true;
	} else
	{
		return false;
	}
}
