#ifndef RANDOM_NUM_GEN_H_
#define RANDOM_NUM_GEN_H_

#include <string>
#include <vector>

int getNewKernel(int& starting_kernel, int numerical_distance);
void generateRNGSeedsToFile(std::string filename, int sim_runs, int k_tx_rx_count, int starting_kernel, int numerical_distance);
void readRNGSeedsFromFile(std::string filename, std::vector<std::vector<int>>& result);

//Class representing RNG generators with
//functions representing various distributions:
//Uniform, exponential, Zero-One

class RandomNumGen
{

public:
	RandomNumGen(int kernel);
	~RandomNumGen();

	//random number generation according to specified distribution
	double rand();
	double rand(int min, int max);
	int randRoundedInt(int min, int max);
	double randExp(double lambda);
	int randExpRoundedInt(double lambda);
	bool randZeroOne(double probability);

	int get_kernel() const;
	void set_k_kernel(int kernel);
private:
	//kernel used in generation of numbers
	//all RNG functions share the same kernel
	int kernel_;

	//Numeric constants used in calculation
	//and setup of the uniform RNG
	const double kM_ = 2147483647.0;
	static const int kA = 16807;
	static const int kQ_ = 127773;
	static const int kR_ = 2836;
};

#endif
