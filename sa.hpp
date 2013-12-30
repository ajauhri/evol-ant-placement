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
    float m_temp_pop_factor;
    
    /* Computing initial temp data
    For description of these variables, refer to algorithm described in http://cs.stackexchange.com/questions/11126/initial-temperature-in-simulated-annealing-algorithm  */
    class transition
    {
        public:
        individual_ptr m_min;
        individual_ptr m_max;
    };
    typedef boost::shared_ptr<transition> transition_ptr;
    std::vector<transition_ptr> m_S;
    float m_accept_prob;
    float m_e;
    float m_p;
    /*******************/

    std::vector<position_ptr> mutate_pos_once(std::vector<position_ptr>&);
    void evaluate(unsigned int, individual_ptr&);
    void run_simulation(unsigned int);
    void compute_temp();


public:
	sa(std::string);
	~sa(void);

	void setup_algo_params();
	void run();
};
#endif
