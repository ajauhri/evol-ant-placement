#ifndef SA_HPP_INCLUDED
#define SA_HPP_INCLUDED
#include "algorithm.hpp"
	
class sa :
	public algorithm
{
private:
	typedef algorithm super;
	unsigned int iterations;
	float initial_temperature;
	float temperature_factor;
	unsigned int iterations_per_temperature_change;
	float convergence_factor;
public:
	sa(void);
	~sa(void);

	void setup_algo_params();
	//void run();
};
#endif
