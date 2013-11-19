#if 0
#include "sa.hpp"
#include "aapot_resources.hpp"
#include<iostream>

namespace
{
	char const *iterations_s = "iterations";
	char const *initial_temperature_s = "initial_temperature";
	char const *temperature_factor_s = "temperature_factor";
	char const *iterations_per_temperature_change_s = "iterations_per_temperature_change";
	char const *convergence_factor_s = "convergence_factor";
}

sa::sa(std::string aapot_file, std::string config_file) : super(aapot_file, config_file)
{}

/**
* @desc Loads parameters for the simulated annealing algorithm  
*/
void sa::setup_algo_params()
{
	try
	{
		algorithm::setup_algo_params();
		this->iterations = atoi(aapot_resources::get_first_attribute(this->algo_node, iterations_s)->value());
		this->initial_temperature = atof(aapot_resources::get_first_attribute(this->algo_node, initial_temperature_s)->value());
		this->temperature_factor = atof(aapot_resources::get_first_attribute(this->algo_node, temperature_factor_s)->value());
		this->iterations_per_temperature_change = atoi(aapot_resources::get_first_attribute(this->algo_node, iterations_per_temperature_change_s)->value());
		this->convergence_factor = atof(aapot_resources::get_first_attribute(this->algo_node, convergence_factor_s)->value());
		std::cout<<"Completed SA parameter setup"<<std::endl;
	}
	catch (const aapot_resources::XMLParseException &e)
	{
		std::cerr<<e.what()<<"\n";
	}
}

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
