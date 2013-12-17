#ifndef SA_HPP_INCLUDED
#define SA_HPP_INCLUDED
#include<algorithm.hpp>
	
class sa :
	public algorithm
{
private:
	unsigned int m_iterations;
	float m_init_temp;
	float m_cooling_factor;
	unsigned int m_iterations_per_temperature_change;
	float m_convergence_factor;
    float m_converged_iterations;
    individual_ptr m_p_parent;
    float m_best_fitness;

    std::vector<position_ptr> mutate_pos(std::vector<position_ptr>&);
    void evaluate(unsigned int, individual_ptr&);
    void run_simulation(unsigned int);

public:
	sa(std::string);
	~sa(void);

	void setup_algo_params();
	void run();
};
#endif
