#include "random_num_gen.h"
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <fstream>      
#include <iostream>    
#include <iterator>     
#include <string>
#include <vector>
#include <sstream>

//function preparing a list of RNG kernels to be used in simulation.
//after providing number or simulation runs and RX&TX count kernels are generated in lines
//each single line == single simulation run
void generateRNGSeedsToFile(std::string filename, int sim_run_count, int k_tx_rx_count, int starting_kernel, int numerical_distance)
{
	//prime instance that the kernels will be extracted from
	auto* prime_rng = new RandomNumGen(starting_kernel);
	//contener for a "line" of seed values
	std::vector<int> seed_set;
	std::ofstream rng_seeds_file(filename);
	//each simulation each set of seeds - line
	for(int i = 0; i< sim_run_count; ++i)
	{
		//rng seed count for PacketGen, TransmissionTime, SleepTime and single TER (channel error)
		for (int j = 0; j< 3*k_tx_rx_count+1; ++j)
		{
			//each kernel separated by a numerical_distance of generations
			for(int k =0; k<numerical_distance; ++k)
			{
				prime_rng->rand();
			} 
			seed_set.push_back(prime_rng->get_kernel());
		}
		//save all to a single line after generation and reset
		std::ostream_iterator<int> output_iterator(rng_seeds_file, ";");
		std::copy(seed_set.begin(), seed_set.end(), output_iterator);
		seed_set.clear();
		rng_seeds_file << std::endl;
	}
	delete prime_rng;
}


//Read list of RNG seeds from file
void readRNGSeedsFromFile(std::string filename, std::vector<std::vector <int>>& result)
{
	std::ifstream rng_seeds_file(filename);
	//for whole file do
	while (rng_seeds_file)
	{
		//extract single line of chars
		std::string single_line;
		if (!getline(rng_seeds_file, single_line)) break;

		//create stringstream instance to operate on this line
		std::stringstream  stream_of_chars(single_line);

		std::vector <int> line_of_values;
		//as long as there are any chars remaining
		while (stream_of_chars)
		{
			std::string single_value;
			//extract each string that ends with delimiter ;
			if (!getline(stream_of_chars, single_value, ';')) break;
			//cast to int and push to vector
			line_of_values.push_back(stoi(single_value));
		}
		result.push_back(line_of_values);
	}
	if (!rng_seeds_file.eof())
	{
		std::cout << "ERROR" << std::endl;
		throw;
	}
}

int getNewKernel(int& starting_kernel, int numerical_distance)
{
	return true;
}

RandomNumGen::RandomNumGen(int kernel)
{
	kernel_ = kernel;
}

RandomNumGen::~RandomNumGen() = default;

//uniform random number generation within range [0;1]
double RandomNumGen::rand()
{
	const int kH = kernel_ / kQ_;
	
	kernel_ = kA * (kernel_ - kQ_ * kH) - kR_ * kH;
	if (kernel_ < 0)
	{
		kernel_ = kernel_ + static_cast<int>(kM_);
	}
	return kernel_ / kM_;
}

//uniform random number generation within range [min;max]
double RandomNumGen::rand(int min, int max)
{
	return rand()*(max-min)+min;
}

//uniform random rounded to integer number generation within range [min;max]
int RandomNumGen::randRoundedInt(int min, int max)
{
	return static_cast<int>(floor(rand()*(max - min + 1)) + min);
}

//exponential random number generation with intensity "lambda"
double RandomNumGen::randExp(double lambda)
{
	return -(1.0 / lambda) * log(rand());
}

//exponential random number generation with intensity "lambda"
int RandomNumGen::randExpRoundedInt(double lambda)
{
	return static_cast<int>(floor(-(1.0 / lambda) * log(rand())));
}

//zero-one random number generation with likelihood of 1 (success) defined by param: "probability"
bool RandomNumGen::randZeroOne(double probability)
{
	if(rand() < probability)
	{
		return true;
	}
	
	return false;
}

//kernel getter
int RandomNumGen::get_kernel() const
{
	return kernel_;
}

void RandomNumGen::set_k_kernel(int kernel)
{
	kernel_ = kernel;
}

