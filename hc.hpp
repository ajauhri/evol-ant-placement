#ifndef HC_HPP_INCLUDED
#define HC_HPP_INCLUDED
#include "algorithm.hpp"
	
class hc :
	public algorithm
{
private:
	float m_convergence_factor;
	unsigned int m_iterations;
    individual_ptr m_p_parent;
    float m_best_fitness;
    float m_converged_iterations;
    
    void evaluate(unsigned int, individual_ptr&);
    void run_simulation(unsigned int);
public:
	hc(std::string);
	~hc(void);

	void setup_algo_params();
    void run(unsigned int);
};
#endif
