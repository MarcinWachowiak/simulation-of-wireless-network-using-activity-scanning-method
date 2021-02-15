#include "remove_lost_packet_event.h"
#include "simulation.h"

//Class representing event of removing packet
//after all attempts to transmit it failed

RemoveLostPacketEvent::RemoveLostPacketEvent(int time,
	Simulation* simulation,
	Transmitter* transmitter) :
	Event(time, simulation),
	transmitter_(transmitter)
{
}

RemoveLostPacketEvent::~RemoveLostPacketEvent()
{
}

bool RemoveLostPacketEvent::isTriggered()
{
	if(transmitter_->is_listening_finished() && !transmitter_->was_tran_ok() && transmitter_->get_curr_retran_num() >= transmitter_->get_k_max_retran_count()) {
		return true;
	} else
	{
		return false;
	}
}

void RemoveLostPacketEvent::execute()
{
	try
	{
		Packet* tmp_packet = transmitter_->get_buffer().front();
		if (simulation_->isInitialPhaseOver())
		{
			//get ptr to stats to ease use
			Statistics* statistics_ptr = simulation_->get_wireless_network()->get_statistics();

			size_t generation_timestamp = tmp_packet->get_gen_timestamp();
			size_t packet_await_time = transmitter_->get_transmit_timestamp() - generation_timestamp;
			
			//add trans/packet values to global results
			statistics_ptr->get_tot_packet_await_time() += packet_await_time;
			statistics_ptr->get_lost_packets_vector().at(transmitter_->get_k_tx_id())++;
			//time packet spend in a queue;
			
		}

		simulation_->get_logger()->debug("[{0}]\tFailed final PACKET retransmission num: {1} by TX id:{2}", simulation_->get_sim_clock(), transmitter_->get_curr_retran_num(), transmitter_->get_k_tx_id());
		
		//remove packet
		transmitter_->get_buffer().pop();
		delete tmp_packet;
		
		//reset temporary TX flags
		transmitter_->set_was_tran_ok(false);
		transmitter_->set_curr_cln_time_down_counter(transmitter_->get_k_tx_init_down_counter_val());
		transmitter_->set_curr_retran_num(0);
		transmitter_->set_was_channel_empty(false);
		transmitter_->set_is_listening_finished(false);
		transmitter_->set_is_active(true);
		transmitter_->set_is_probing_active(false);
		transmitter_->set_is_channel_observed(false);
		transmitter_->set_transmit_timestamp(0);
		//reactivate channel observation
		transmitter_->set_is_busy(false);

		
	} catch (...)
	{
		simulation_->get_logger()->error("[{0}]\tError in remove lost packet event handling", simulation_->get_sim_clock(), transmitter_->get_curr_retran_num());
	}
	
}
