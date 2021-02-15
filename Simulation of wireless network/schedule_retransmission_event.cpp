#include "simulation.h"
#include "schedule_retransmission_event.h"
#include "end_tx_sleep_event.h"
#include "probe_channel_state_event.h"

//Class representing a conditional event of scheduling
//retransmission after not receiving ack when listening for it is finished
//and retransmit is possible, current retransmission number is below the maximum value

ScheduleRetransmissionEvent::ScheduleRetransmissionEvent(
	int time,
	Simulation* simulation,
	Transmitter* transmitter,
	RandomNumGen* sleep_time_rng,
	EndTxSleepEvent* end_tx_sleep_event,
	ProbeChannelStateEvent* probe_channel_state_event) :
	Event(time,simulation),
	transmitter_(transmitter),
	sleep_time_rng_(sleep_time_rng),
	end_tx_sleep_event_(end_tx_sleep_event),
	probe_channel_state_event_(probe_channel_state_event)
{
}

ScheduleRetransmissionEvent::~ScheduleRetransmissionEvent()
{
}

void ScheduleRetransmissionEvent::execute()
{
	try
	{
		//retransmit possible
		//generate sleep time multiplier R  = <0,(2^retran_num-1)>;
		int  R_multiplier = (sleep_time_rng_->randRoundedInt(0,static_cast<int>(pow(2,transmitter_->get_curr_retran_num())-1))); // 
		//calculate sleep time equal to: R*CTPk
		int sleep_time = R_multiplier * transmitter_->get_transmission_time();
		
		//put TX to sleep (schedule next channel check)
		//when sleep time == 0 plan next counter update
		//on next clock tick not in the current moment (0)!
		if (sleep_time == 0)
		{
			end_tx_sleep_event_->set_time(simulation_->get_minimal_time_step());
		}
		else
		{
			end_tx_sleep_event_->set_time(sleep_time);
		}
		//deactivate probing
		probe_channel_state_event_->set_time(-1);
		//reset local tx down counter
		transmitter_->set_was_channel_empty(false);
		//deactivate TX for sleep time
		transmitter_->set_is_active(false);
		transmitter_->set_is_busy(false);
		transmitter_->set_is_listening_finished(false);
		transmitter_->set_is_channel_observed(false);
		transmitter_->set_curr_cln_time_down_counter(transmitter_->get_k_tx_init_down_counter_val());

		simulation_->get_logger()->debug("[{0}]\tCorrupted PACKET transmission num: {1} by TX id: {2}, going to sleep for: {3} ", simulation_->get_sim_clock(), transmitter_->get_curr_retran_num(), transmitter_->get_k_tx_id(), sleep_time);
	} catch (...)
	{
		simulation_->get_logger()->error("[{0}]\tError while handling ScheduleRetransmissionEvent", simulation_->get_sim_clock());
	}
}

bool ScheduleRetransmissionEvent::isTriggered()
{
	if(transmitter_->is_listening_finished()&& !transmitter_->was_tran_ok() && transmitter_->get_curr_retran_num() < transmitter_->get_k_max_retran_count())
	{
		return true;
	} else
	{
		return false;
	}
}
