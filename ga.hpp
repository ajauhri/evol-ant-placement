#ifndef GA_HPP_INCLUDED
#define GA_HPP_INCLUDED
#include<algorithm.hpp>
#include<individual.hpp>
#include<vector>	
class ga : public algorithm
{
private:
	unsigned int m_population_size;
	unsigned int m_generations;
	unsigned int m_elitism;
	unsigned int m_tournament_size;
	float m_recombination;
    std::vector<individual_ptr> m_pop;
	
    individual_ptr tour();
    void evaluate_gen(unsigned int);
    void create_generation(unsigned int);
    void select();

    void save_best_nec();
    void run_simulation(unsigned int);

public:
	ga(std::string);
	virtual ~ga(void);

	void setup_algo_params();
	void run();
};
#endif
