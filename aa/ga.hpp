#ifndef GA_HPP_INCLUDED
#define GA_HPP_INCLUDED
#include "selector.hpp"
#include "algorithm.hpp"
#include "rapidxml/rapidxml.hpp"
#include "individual.hpp"
#include<vector>	
using namespace rapidxml;
class ga : public algorithm
{
private:
	typedef algorithm super;
	unsigned int population_size;
	unsigned int generations;
	unsigned int elitism;
	unsigned int tournament_size;
	float recombination_probability;
	std::vector<individual_ptr> pop;
	individual_ptr tour();

public:
	ga(std::string, std::string);
	~ga(void);

	void setup_algo_params();
	void run();
	void select();
};
#endif
