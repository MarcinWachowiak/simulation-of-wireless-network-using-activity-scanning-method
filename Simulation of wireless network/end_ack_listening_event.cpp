#include "end_ack_listening_event.h"
#include "simulation.h"

//time event representing end of waiting/listening for ACK

EndAckListeningEvent::EndAckListeningEvent(int time,
	Simulation* simulation,
	Transmitter* transmitter) :
	Event(time,simulation),
	transmitter_(transmitter)
{
}

EndAckListeningEvent::~EndAckListeningEvent()
{
}

void EndAckListeningEvent::execute()
{
	try
	{
		//no ACK => transmission failed
		//increment retransmission number/count
		transmitter_->get_curr_retran_num()++;
		transmitter_->set_is_listening_finished(true);
		//disable re trigger of the event after it has been handled
		time_ = -1;
		simulation_->get_logger()->debug("[{0}]\tFailed PACKET retransmission num: {1} by TX id:{2}", simulation_->get_sim_clock(), transmitter_->get_curr_retran_num(), transmitter_->get_k_tx_id());
		
	}catch (...)
	{
		simulation_->get_logger()->error("[{0}]\tError while handling EndAckListeningEvent", simulation_->get_sim_clock());
	}
}

