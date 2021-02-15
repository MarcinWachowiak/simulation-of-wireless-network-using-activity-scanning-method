#include "simulation.h"
#include "start_retransmission_event.h"
#include "reduce_tx_down_counter_event.h"
#include "probe_channel_state_event.h"

//Class representing conditional event of restarting
//transmission by TX has method checking trigger conditions
//after triggering and starting proper events are planned to
//handle ends of transmission and listening

StartRetransmissionEvent::StartRetransmissionEvent(int time,
	Simulation* simulation,
	Transmitter* transmitter,
	RandomNumGen* transmission_time_rng,
	EndAckListeningEvent* end_ack_listening_event,
	EndPacketTransmissionEvent* end_packet_transmission_event,
	ProbeChannelStateEvent* probe_channel_state_event
)
	: Event(time, simulation),
	transmitter_(transmitter),
	transmission_time_rng_(transmission_time_rng),
	end_ack_listening_event_(end_ack_listening_event),
	end_packet_transmission_event_(end_packet_transmission_event),
	probe_channel_state_event_(probe_channel_state_event)
{
}

StartRetransmissionEvent::~StartRetransmissionEvent()
{
}

void StartRetransmissionEvent::execute()
{
	try
	{
		//Generate transmission time using RNG
		//CTPk value 10,20,30...,100ms
		int transmission_time = 10 * transmission_time_rng_->randRoundedInt(1, 10);
		//save transmission time in transmitter
		transmitter_->set_transmission_time(transmission_time);
		//create new transmission
		auto transmission_to_send = new Transmission(transmission_time, true, transmitter_, transmitter_->get_coupled_receiver());
		//deacitvate probing 
		probe_channel_state_event_->set_time(-1);

		//place transmission in channel
		transmitter_->set_is_probing_active(false);
		transmitter_->set_is_busy(true);
		transmitter_->set_is_channel_observed(false);
		simulation_->get_wireless_network()->get_channel()->get_transmissions().push_back(transmission_to_send);
		//plan events
		//change (activate event in a list) time of end transmission to generated time
		end_packet_transmission_event_->set_time(transmission_time);
		end_packet_transmission_event_->set_transmission(transmission_to_send);
		//plan checking channel by TX on time transmit + CTIZ(ack time)
		end_ack_listening_event_->set_time(transmission_time + simulation_->get_wireless_network()->get_k_ack_msg_time() + simulation_->get_minimal_time_step());

		simulation_->get_logger()->debug("[{0}]\tStarting PACKET retransmission TX id:{1}, time length:{2}", simulation_->get_sim_clock(), transmitter_->get_k_tx_id(), transmitter_->get_transmission_time());
	}
	catch (...)
	{
		simulation_->get_logger()->error("[{0}]\tStarting retransmission failed TX id:{1}", simulation_->get_sim_clock(), transmitter_->get_k_tx_id());
	}
}

bool StartRetransmissionEvent::isTriggered()
{
	//if down counter timer has reached 0 trigger conditional
	//event start transmission
	if (transmitter_->get_curr_cln_time_down_counter() == 0 && transmitter_->get_transmit_timestamp() != 0 && !transmitter_->is_busy())
	{
		//simulation_->get_logger()->debug("[{0}]\tTriggered: starting PACKET transmission TX id:{1}", simulation_->get_sim_clock(), transmitter_->get_k_tx_id());
		return true;
	}
	else
	{
		//simulation_->get_logger()->debug("\t[{0}]\tNot triggered: starting PACKET transmission TX id:{1}", simulation_->get_sim_clock(), transmitter_->get_k_tx_id());
		return false;
	}
}


