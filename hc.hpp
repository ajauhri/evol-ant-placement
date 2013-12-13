#ifndef HC_HPP_INCLUDED
#define HC_HPP_INCLUDED
#include "algorithm.hpp"
	
class hc :
	public algorithm
{
private:
	float convergence_factor;
	int iterations;
    
    void run_simulation(unsigned int);
    void save_best_nec();
public:
	hc(std::string);
	~hc(void);

	void setup_algo_params();
    void run();
};
#endif
