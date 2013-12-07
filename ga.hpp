#ifndef GA_HPP_INCLUDED
#define GA_HPP_INCLUDED
#include "algorithm.hpp"
#include "individual.hpp"
#include<vector>	
class ga : public algorithm
{
private:
	typedef algorithm super;
	unsigned int population_size;
	unsigned int generations;
	unsigned int elitism;
	unsigned int tournament_size;
	float recombination;
	std::vector<individual_ptr> pop;
	//individual_ptr tour();

public:
	ga(std::string);
	~ga(void);

	void setup_algo_params();
	void run();
	//void select();
};
#endif
