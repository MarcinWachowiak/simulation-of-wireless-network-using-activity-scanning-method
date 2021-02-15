#include "simulation.h"
#include "transmit_ack_event.h"
#include "end_ack_transmission_event.h"

//Class representing conditional event of sending ACK by RX
//has method checking event trigger conditions
//and handling sending response to the TX
//if reception was successful

TransmitAckEvent::TransmitAckEvent(
	int time,
	Simulation* simulation,
	Receiver* receiver,
	EndAckTransmissionEvent* end_ack_transmission_event
	)
	: Event(time, simulation),
	receiver_(receiver),
	end_ack_transmission_event_(end_ack_transmission_event)
{
}

TransmitAckEvent::~TransmitAckEvent()
{
}

void TransmitAckEvent::execute()
{
	try
	{
		//increment successful transmission count
		if(simulation_->isInitialPhaseOver())
		{
			simulation_->get_wireless_network()->get_statistics()->get_successfully_received_packets_vector().at(receiver_->get_k_rx_id())++;
		}
		
		//generate ACK msg
		//time length CTIZ, addr: to TX...from RX,
		int ack_msg_time = simulation_->get_wireless_network()->get_k_ack_msg_time();

		auto transmission_to_send = new Transmission(ack_msg_time, false, receiver_->get_coupled_transmitter(), receiver_);

		//place transmission in channel
		simulation_->get_wireless_network()->get_channel()->get_transmissions().push_back(transmission_to_send);
		//plan=>change time=>activate event end of transmission;
		end_ack_transmission_event_->set_time(ack_msg_time);
		end_ack_transmission_event_->set_transmission(transmission_to_send);
		simulation_->get_logger()->debug("[{0}]\tStarting ACK transmission by RX id:{1}, time length:{2}", simulation_->get_sim_clock(), receiver_->get_k_rx_id(),simulation_->get_wireless_network()->get_k_ack_msg_time());

		//reset receiver state flag
		receiver_->set_was_packet_received(false);
		receiver_->set_was_packet_uncorrupted(false);
		receiver_->set_was_uncorrupted_by_channel(false);
	} catch (...)
	{
		simulation_->get_logger()->error("[{0}]\tTransmitting ACK failed RX id:{1}", simulation_->get_sim_clock(), receiver_->get_k_rx_id());
	}
	
}

bool TransmitAckEvent::isTriggered()
{
	
	if(receiver_->was_packet_received() && receiver_->was_packet_uncorrupted() && receiver_->was_uncorrupted_by_channel())
	{
		//transmission uninterrupted
		simulation_->get_logger()->debug("[{0}]\tTriggered: transmitting ACK RX id:{1}", simulation_->get_sim_clock(), receiver_->get_k_rx_id());
		return true;
		
	}else {
		return false;
	}
}


