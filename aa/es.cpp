#include "es.hpp"
#include "aapot_resources.hpp"
#include <iostream>

namespace 
{
	char const *mu_s = "mu";
	char const *lambda_s = "lambda";
}


es::es(std::string aapot_file, std::string config_file) : super(aapot_file, config_file)
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
		this->mu = atoi(aapot_resources::get_first_attribute(this->algo_node, mu_s)->value());
		this->lambda = atoi(aapot_resources::get_first_attribute(this->algo_node, lambda_s)->value());
		std::cout<<"Completed ES parameter setup"<<std::endl;
	}
	catch (const aapot_resources::XMLParseException &e)
	{
		std::cerr<<e.what()<<"\n";
	}
}

/**
* @desc Implements logic for ES runs
*/
void es::run()
{
	algorithm::setup_run_context();
	
}

es::~es(void)
{
}
