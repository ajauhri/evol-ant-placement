#include<iostream>
#include "hc.hpp"
#include "eap_resources.hpp"
#include "lua_cmds.hpp"

namespace 
{
	char const *iterations_s = "iterations";
	char const *convergence_factor_s = "convergence_factor_s";
}

hc::hc() : super()
{
}


/**
* @desc Loads parameters for the hill climber algorithm  
*/
void hc::setup_algo_params()
{
	try
	{
		algorithm::setup_algo_params();
		this->iterations = eap::get_fvalue(iterations_s);
		this->convergence_factor = eap::get_fvalue(convergence_factor_s);
		std::cout<<"Completed HC parameter setup"<<std::endl;
	}
	catch (const eap::InvalidStateException &e)
	{
		std::cerr<<e.what()<<"\n";
	}
}

#if 0
/**
* @desc Implements logic for HC runs
*/
void hc::run()
{
	algorithm::setup_run_context();
	
}

#endif

hc::~hc(void)
{
}
