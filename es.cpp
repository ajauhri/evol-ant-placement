#include<es.hpp>
#include<eap_resources.hpp>
#include<lua_cmds.hpp>
#include <iostream>

namespace 
{
	char const *mu_s = "mu";
	char const *lambda_s = "lambda";
}


es::es(std::string lua_file) : algorithm(lua_file)
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

/**
* @desc Implements logic for ES runs
*/
void es::run()
{
	
}

void es::run_simulation(unsigned int id)
{
}

void es::save_best_nec()
{}

es::~es(void)
{
}

