#ifndef SA_HPP_INCLUDED
#define SA_HPP_INCLUDED
#include<algorithm.hpp>
	
class sa :
	public algorithm
{
private:
	unsigned int m_iterations;
	float m_initial_temperature;
	float m_temperature_factor;
	unsigned int m_iterations_per_temperature_change;
	float m_convergence_factor;
    float m_converged_iterations;
public:
	sa(std::string);
	~sa(void);

	void setup_algo_params();
	void run();
    void run_simulation(unsigned int);
};
#endif
