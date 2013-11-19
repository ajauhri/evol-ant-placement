#if 0
#ifndef HC_HPP_INCLUDED
#define HC_HPP_INCLUDED
#include "algorithm.hpp"
#include "rapidxml/rapidxml.hpp"
	
using namespace rapidxml;
class hc :
	public algorithm
{
private:
	typedef algorithm super;
	float convergence_factor;
	int iterations;

public:
	hc(std::string, std::string);
	~hc(void);

	void setup_algo_params();
	void run();
};
#endif

#endif
