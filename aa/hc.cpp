#include<iostream>
#include "hc.hpp"
#include "aapot_resources.hpp"

namespace 
{
	char const *iterations_s = "iterations";
	char const *convergence_factor_s = "convergence_factor_s";
}

hc::hc(std::string aapot_file, std::string config_file) : super(aapot_file, config_file)
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
		this->iterations = atoi(aapot_resources::get_first_attribute(this->algo_node, iterations_s)->value());
		this->convergence_factor = atof(aapot_resources::get_first_attribute(this->algo_node, convergence_factor_s)->value());
		std::cout<<"Completed HC parameter setup"<<std::endl;
	}
	catch (const aapot_resources::XMLParseException &e)
	{
		std::cerr<<e.what()<<"\n";
	}
}

/**
* @desc Implements logic for HC runs
*/
void hc::run()
{
	algorithm::setup_run_context();
	
}


hc::~hc(void)
{
}
