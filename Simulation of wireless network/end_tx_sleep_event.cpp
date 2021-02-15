#include "end_tx_sleep_event.h"
#include "simulation.h"

//class representing time event of wake up from sleep
//re enabling of normal TX activity

EndTxSleepEvent::EndTxSleepEvent(
	int time,
	Simulation* simulation,
	Transmitter* transmitter) :
	Event(time,simulation),
	transmitter_(transmitter)
{
}

EndTxSleepEvent::~EndTxSleepEvent()
{
}

void EndTxSleepEvent::execute()
{
	try
	{
		//reactivate tx to enable other conditional events execution
		transmitter_->set_is_active(true);
		simulation_->get_logger()->debug("[{0}]\tSleep finished for TX id:{1}", simulation_->get_sim_clock(), transmitter_->get_k_tx_id());
		time_ = -1;
		
	}catch (...)
	{
		simulation_->get_logger()->error("[{0}]\tError while handling EndTxSleepEvent", simulation_->get_sim_clock());
	}
}
