#ifndef HC_HPP_INCLUDED
#define HC_HPP_INCLUDED
#include "algorithm.hpp"
	
class hc :
	public algorithm
{
private:
	typedef algorithm super;
	float convergence_factor;
	int iterations;

public:
	hc(std::string);
	~hc(void);

	void setup_algo_params();
    void run();
    void run_simulation(unsigned int);
};
#endif
