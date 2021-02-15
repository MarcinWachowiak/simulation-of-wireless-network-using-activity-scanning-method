#include "random_num_gen.h"
#include "simulation.h"
#include "statistics.h"
#include  "wireless_network.h"
#include "spdlog/spdlog.h"
#include <fstream>   
#include <iomanip>
#include <algorithm>
#include <sstream>

//Created by: Marcin Wachowiak, Poland

int main(int argc, char * argv[])
{
	//>>>>LOGGER INSTANTIATION: CONSOLE AND FILE<<<<
	//create console sink instance (colored one)
	auto console_sink = std::make_shared<spdlog::sinks::wincolor_stdout_sink_st>();
	//***console level set to debug to supervise event class testing (regular use set: err)***
	console_sink->set_level(spdlog::level::info);
	//create file sink instance, specify output file
	auto file_sink = std::make_shared<spdlog::sinks::simple_file_sink_st>("results/simulation_log.txt", true);
	//set level to all - every msg will be saved
	file_sink->set_level(spdlog::level::info);
	//combine two sinks into one represented by "logger" instance
	spdlog::sinks_init_list sink_list = { file_sink, console_sink };
	auto *combined_logger = new spdlog::logger("Simulation", sink_list.begin(), sink_list.end());
	combined_logger->set_level(spdlog::level::info);
	combined_logger->set_pattern("[%^%l%$] %v");

	//Common simulation constants
	const int kAckMsgTime = 10; //1ms
	//smallest time step - grain
	//the packet generators will be trimmed to 
	const int kMinimalTimeStep = 1; //0,1ms
	const int kChannelCheckInterval = 5; //0,5ms
	const double kChannelProbabilityOfSuccess = 0.8;
	
	//Project dependent constants
	const int kRxTxCount = 16;
	const int kMaxRetranCount = 6;
	const int kRequiredFreeTimeDifs = 50; //5ms

	//SIM RUN/REPETITION PARAMS
	int kSimulationRuns = 1;
	int kRepeatCount = 15;
	
	//simulation will be evaluated ove this value
	double packet_rng_intensity = 0.0002272;
	int kSimulationTime=16000000;
	int kInitialPhaseTimeLength=6000000;
	
	//RNG SEEDS GENERATIONG
	combined_logger->info("Generating RNG seeds this may take some time...");

	int kStartingKernelVal = 10032000;
	const int kNumericalDistance = 1000000;
	std::string kSeedFilename = "RNG_seed_list.txt";
	//Create file with RNG seeds to be used in simulation
	generateRNGSeedsToFile(kSeedFilename, kSimulationRuns*kRepeatCount, kRxTxCount, kStartingKernelVal, kNumericalDistance);
	//vector of seeds for each simulation passed by ref to reading function
	std::vector <std::vector <int> > RNG_seeds;
	//read seeds from file
	readRNGSeedsFromFile(kSeedFilename, RNG_seeds);
	//RNG SEEDS GENERATION END
	combined_logger->info("Finished generating and reading RNG seeds!");
	
	//SIMULATION MODEL INSTANTIATION
	auto* my_statistics = new Statistics();
	auto my_wireless_network = new WirelessNetwork(kRxTxCount, kMaxRetranCount, my_statistics,kRequiredFreeTimeDifs+kChannelCheckInterval,kAckMsgTime );
	auto *my_simulation = new Simulation(
		kInitialPhaseTimeLength,
		kSimulationTime,
		kMinimalTimeStep,
		kChannelCheckInterval, 
		packet_rng_intensity, 
		kChannelProbabilityOfSuccess,
		my_wireless_network,
		combined_logger);
	//link statistics to main simulation instance
	my_statistics->set_simulation_ptr(my_simulation);
	//SIMULATION MODEL INSTANTIATION END
	
	//INIT PHASE 
	char kSimResultsFilename[] = "results/Simulation_results.txt";
	remove(kSimResultsFilename);
	
	my_simulation->set_save_initial_phase_data(false);
	
	//start indexing
	my_simulation->set_simulation_run_index(0);
	//write first log to file - descriptive
	std::fstream sim_results_file(kSimResultsFilename, std::fstream::out | std::fstream::app);
	sim_results_file.close();
	sim_results_file.open(kSimResultsFilename, std::fstream::out | std::fstream::app);
	sim_results_file << "Simulation run index;\tPacket gen intensity(L);\tChannel probability of success (TER);\tInitial phase length;\tSimulation time;\tTX and RX pair count;\tMaximum retransmission number (LR);\tRequired channel free time (DIFS)[quants = 0,1ms];\tResults;\tAverage packet error rate;\tMaximum packet error rate;\tAverage retransmission count;\tSystem throughput [packets/s];\tAverage packet delay time [ms];\tAverage packet await time [ms];\n";
		sim_results_file.close();

	//run simulation in loop 
	for (int i = 0; i < kSimulationRuns; i++)
	{
		
		for (int j = 0; j < kRepeatCount; j++)
		{
			my_simulation->get_simulation_run_index()++;
			//INIT PHASE DATA FILE
			if(my_simulation->is_save_initial_phase_data())
			{
				std::string init_phase_filename = "results/init_phase_eval/Init_phase_file_";
				std::stringstream ss;
				ss << std::fixed << std::setprecision(7) << my_simulation->get_k_packet_rng_intensity();
				std::string tmp_lambda = ss.str();
				std::replace(tmp_lambda.begin(), tmp_lambda.end(), '.', '_');
				init_phase_filename += std::to_string(my_simulation->get_simulation_run_index()) + "_" + tmp_lambda + "_" + std::to_string(j) + ".txt";
				my_simulation->set_init_phase_data_filename(init_phase_filename);
			}
			
			//load RNG seeds
			my_simulation->loadRNGSeeds(RNG_seeds.at(i*kRepeatCount + j));
			my_simulation->resetAndInitialize();
			my_simulation->run(false);
			
			//simulation finished save results to file
			sim_results_file.open(kSimResultsFilename, std::fstream::out | std::fstream::app);
			sim_results_file << std::fixed << std::setprecision(6);
			sim_results_file << (i + 1) << ";\t" << my_simulation->get_k_packet_rng_intensity() << ";\t" << my_simulation->get_k_channel_probability_of_success() << ";\t" << my_simulation->get_k_initial_phase_time_length() << ";\t" << my_simulation->get_target_sim_time() << ";\t" << my_wireless_network->get_k_rx_tx_count() << ";\t" << my_wireless_network->get_transmitters().at(0)->get_k_max_retran_count() << ";\t" << (my_wireless_network->get_k_tx_init_down_counter_val() - my_simulation->get_channel_check_interval()) << ";\t" << "" << ";\t" << my_statistics->getAvgPacketErrorRate() << ";\t" << my_statistics->getMaxPacketErrorRate() << ";\t" << my_statistics->getAvgRetransmissionCount() << ";\t" << (my_statistics->getTotalSystemThroughputPerSimQuant() * 10000) << ";\t" << (my_statistics->getAvgPacketDelayTime() / 10) << ";\t" << (my_statistics->getAvgPacketAwaitTime() / 10) << "\n";
			sim_results_file.close();
		}
		//my_simulation->get_k_packet_rng_intensity() += 0.000005;
		//my_simulation->get_k_channel_probability_of_success() += 0.05;
	}

	delete my_simulation;
	return 0;
}

//*
//**
//***
//ADDITIONAL CRITERIA TO TERMINATE SIMULATION
//by default additional terminators are inactive
//possible options: (all base on statistical results)
//**terminators are activated only after initial phase is over**
//
//my_simulation->set_target_total_packets_received_count(500);
//my_simulation->set_target_total_lost_packets_count(100);
//my_simulation->set_target_avg_packet_error_rate(0.1);
//my_simulation->set_target_avg_retransmission_count(3.0);
//***
//**
//*

//Console IO simulation preset
/*
	combined_logger->info("Input simulation time length (1->0,1ms):");
	std::cin >> kSimulationTime;
	combined_logger->info("Input initial phase time length (1->0,1ms):");
	std::cin >> kInitialPhaseTimeLength;
	combined_logger->info("Starting simulation...");
*/

//Created by: Marcin Wachowiak, Poland
