#include "sa.hpp"
#include "eap_resources.hpp"
#include "lua_cmds.hpp"
#include<iostream>

namespace
{
	char const *iterations_s = "iterations";
	char const *initial_temperature_s = "initial_temperature";
	char const *temperature_factor_s = "temperature_factor";
	char const *iterations_per_temperature_change_s = "iterations_per_temperature_change";
	char const *convergence_factor_s = "convergence_factor";
}

sa::sa(std::string lua_file) : super(lua_file)
{}

/**
* @desc Loads parameters for the simulated annealing algorithm  
*/
void sa::setup_algo_params()
{
	try
	{
		algorithm::setup_algo_params();
		this->iterations = eap::get_fvalue(iterations_s); 
		this->initial_temperature = eap::get_fvalue(initial_temperature_s);
		this->temperature_factor = eap::get_fvalue(temperature_factor_s); 
		this->iterations_per_temperature_change = eap::get_fvalue(iterations_per_temperature_change_s); 
		this->convergence_factor = eap::get_fvalue(convergence_factor_s); 
		std::cout<<"Completed SA parameter setup"<<std::endl;
	}
	catch (const eap::InvalidStateException &e)
	{
		std::cerr<<e.what()<<"\n";
	}
}

#if 0
/**
* @desc Implements logic for SA runs
*/
void sa::run()
{
	algorithm::setup_run_context();
}


sa::~sa(void)
{
}
#endif
