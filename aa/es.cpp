#include "es.hpp"
#include "eap_resources.hpp"
#include "lua_cmds.hpp"
#include <iostream>

namespace 
{
	char const *mu_s = "mu";
	char const *lambda_s = "lambda";
}


es::es() : super()
{
}

/**
* @desc Loads parameters for the evolutionary strategy algorithm  
*/
void es::setup_algo_params()
{
	try
	{
		algorithm::setup_algo_params();
		this->mu = eap::get_fvalue(mu_s); 
		this->lambda = eap::get_fvalue(lambda_s); 
		std::cout<<"Completed ES parameter setup"<<std::endl;
	}
	catch (const eap::InvalidStateException &e)
	{
		std::cerr<<e.what()<<"\n";
	}
}

#if 0
/**
* @desc Implements logic for ES runs
*/
void es::run()
{
	algorithm::setup_run_context();
	
}

#endif
es::~es(void)
{
}

