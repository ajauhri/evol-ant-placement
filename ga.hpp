#ifndef GA_HPP_INCLUDED
#define GA_HPP_INCLUDED
#include<algorithm.hpp>
#include<individual.hpp>
#include<vector>	
class ga : public algorithm
{
private:
	unsigned int population_size;
	unsigned int generations;
	unsigned int elitism;
	unsigned int tournament_size;
	float recombination;
    std::vector<individual_ptr> pop;
	
    individual_ptr tour();
    void run_simulation(unsigned int);
    void evaluate_gen(unsigned int);
    void create_generation(unsigned int);
    void select();

public:
	ga(std::string);
	virtual ~ga(void);

	void setup_algo_params();
	void run();
};
#endif
