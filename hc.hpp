#ifndef HC_HPP_INCLUDED
#define HC_HPP_INCLUDED
#include<map>
#include "algorithm.hpp"
	
class hc :
	public algorithm
{
private:
	float m_convergence_factor;
	unsigned int m_iterations;
    individual_ptr m_p_parent;
    float m_converged_iterations;

    /* for faster iterations */
    std::map<std::string, int> map;
    std::map<int, float> tot;
    std::map<int, float> coup;
    std::map<int, float> rad;
    
    void evaluate(unsigned int, individual_ptr&);
    void run_simulation(unsigned int);
public:
	hc(std::string);
	~hc(void);

	void setup_algo_params();
    void run(unsigned int);
};
#endif
