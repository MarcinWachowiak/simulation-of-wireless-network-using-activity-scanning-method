#include <cmath>
#include "check_comm_status_event.h"
#include "update_tx_down_counter_event.h"

//Regularly planned event class updating transmitter's
//channel free down counters
//self-rescheduling

CheckCommStatusEvent::CheckCommStatusEvent(
	int time, 
	Simulation* simulation,
	Transmitter* transmitter,
	RandomNumGen* sleep_time_rng,
	UpdateTxDownCounterEvent* update_tx_down_counter_event
	)
	: Event(time, simulation),
	transmitter_(transmitter),
	sleep_time_rng_(sleep_time_rng),
	update_tx_down_counter_event_(update_tx_down_counter_event)
{	
}

CheckCommStatusEvent::~CheckCommStatusEvent()
{
}

void CheckCommStatusEvent::execute()
{
	try
	{
		if (transmitter_->was_tran_ok())
		{
			//ACK received => transmission successful
			transmitter_->handlePacketRemoval(simulation_);
			scheduleTxDownCounterUpdate();

			simulation_->get_logger()->debug("\t[{0}]\tSuccessful PACKET transmission by TX id:{1}", simulation_->get_sim_clock(), transmitter_->get_k_tx_id());
		} else
		{
			//no ACK => transmission failed
			//increment retransmission number/count
			transmitter_->get_curr_retran_num()++;

			//check if retransmission is possible
			if(transmitter_->get_curr_retran_num() >= transmitter_->get_k_max_retran_count())
			{
				//retransmit forbidden
				//procedure same as ACK OK (save counters, remove packet reset TX)
				transmitter_->handlePacketRemoval(simulation_);
				scheduleTxDownCounterUpdate();

				simulation_->get_logger()->error("\t[{0}]\tFailed final PACKET retransmission num: {1} by TX id:{2}", simulation_->get_sim_clock(), transmitter_->get_curr_retran_num(), transmitter_->get_k_tx_id());
			}else
			{
				//retransmit possible
				//generate sleep time multiplier R  = <0,(2^retran_num-1)>;
				int  R_multiplier =  (sleep_time_rng_->randRoundedInt(0, static_cast<int>(pow(2,transmitter_->get_curr_retran_num())-1))); // 
				//calculate sleep time equal to: R*CTPk
				int sleep_time = R_multiplier * transmitter_->get_transmission_time();

				//put TX to sleep (schedule next channel check)
				//when sleep time == 0 plan next counter update
				//on next clock tick not in the current moment (0)!
				if(sleep_time==0)
				{
					update_tx_down_counter_event_->set_time(simulation_->get_minimal_time_step());
				} else
				{
					update_tx_down_counter_event_->set_time(sleep_time);
				}
				update_tx_down_counter_event_->set_was_channel_free(false);
				
				//reset local tx down counter
				transmitter_->set_is_busy(false);
				transmitter_->set_curr_cln_time_down_counter(transmitter_->get_k_tx_init_down_counter_val());
				
				simulation_->get_logger()->debug("\t[{0}]\tCorrupted PACKET transmission num: {1} by TX id: {2}, going to sleep for: {3} ", simulation_->get_sim_clock(), transmitter_->get_curr_retran_num(), transmitter_->get_k_tx_id(), sleep_time);
			}	
		}
		//disable re trigger of the event after it has been handled
		time_ = -1;
	}
	catch (...)
	{
		//exception
		simulation_->get_logger()->error("\t[{0}]\tFailed to check communication status for TX id:{1}", simulation_->get_sim_clock(), transmitter_->get_k_tx_id());
	}
		
}

//additional method scheduling proper down counter update
//when there are no packets: update is disabled and will
//reactivate when packet arrives
//when there are packets waiting update
//is planned on next closest moment (+1 clk)
void CheckCommStatusEvent:: scheduleTxDownCounterUpdate()
{
	update_tx_down_counter_event_->set_was_channel_free(false);
	
	if (transmitter_->get_buffer().empty())
	{
		update_tx_down_counter_event_->set_time(-1);
	}
	else
	{
		update_tx_down_counter_event_->set_time(simulation_->get_minimal_time_step());
	}
}