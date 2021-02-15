#include "update_tx_down_counter_event.h"

//Class representing a regular event of updating local TX
//clean channel down counter, depending on a series of conditions
//automatically reschedules itself

UpdateTxDownCounterEvent::UpdateTxDownCounterEvent(
	int time,
	Simulation* simulation,
	Transmitter* transmitter
) : Event(time,simulation),
	transmitter_(transmitter),
	was_channel_free_(false)
{
}

UpdateTxDownCounterEvent::~UpdateTxDownCounterEvent()
{
}

void UpdateTxDownCounterEvent::execute()
{
	try
	{
		if (!transmitter_->get_buffer().empty())
		{
			//check channel state
			if (!simulation_->get_wireless_network()->get_channel()->is_channel_busy())
			{
				if(was_channel_free_)
				{
					//channel empty
					//reduce counter by simulation step
					transmitter_->get_curr_cln_time_down_counter() -= simulation_->get_channel_check_interval();
					simulation_->get_logger()->debug("[{0}]\tChannel empty, counter reduced for TX id:{1}, val: {2}", simulation_->get_sim_clock(), transmitter_->get_k_tx_id(), transmitter_->get_curr_cln_time_down_counter());
				} else
				{
					
				}
			}
			//channel busy
			else
			{
			
			//reset transmitter down counter to default value
			transmitter_->set_curr_cln_time_down_counter(transmitter_->get_k_tx_init_down_counter_val());
			simulation_->get_logger()->debug("[{0}]\tChannel busy, counter reset for TX id:{1} to val: {2}", simulation_->get_sim_clock(), transmitter_->get_k_tx_id(), transmitter_->get_k_tx_init_down_counter_val());
			}
		} else
		{
			simulation_->get_logger()->debug("[{0}]\tCounter update not executed (no packets) for TX id:{1}", simulation_->get_sim_clock(), transmitter_->get_k_tx_id());
		}
		//reschedule this event, each next quant of time 0,5ms
		//plan the next event (repeated each 0,5ms) =s default time step
		time_ = simulation_->get_channel_check_interval();
	} catch (...)
	{
		simulation_->get_logger()->error("\t[{0}]\tCounter update failed for TX id:{1}", simulation_->get_sim_clock(), transmitter_->get_k_tx_id());
	}
	
}

bool& UpdateTxDownCounterEvent::is_was_channel_free()
{
	return was_channel_free_;
}

void UpdateTxDownCounterEvent::set_was_channel_free(bool was_channel_free)
{
	was_channel_free_ = was_channel_free;
}



