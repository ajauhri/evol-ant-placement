#include<sa.hpp>
#include<eap_resources.hpp>
#include<lua_cmds.hpp>
#include<iostream>

namespace
{
	const std::string c_iterations = "iterations";
	const std::string c_init_temp = "init_temp";
	const std::string c_temp_factor = "temp_factor";
	const std::string c_iterations_per_temp_change = "iterations_per_temp_change";
	const std::string c_convergence_factor = "convergence_factor";
}

sa::sa(std::string lua_file) : algorithm(lua_file)
{
}

/**
* @desc Loads parameters for the simulated annealing algorithm  
*/
void sa::setup_algo_params()
{
	try
	{
		algorithm::setup_algo_params();
		m_iterations = eap::get_fvalue(c_iterations); 
		m_initial_temperature = eap::get_fvalue(c_init_temp);
		m_temperature_factor = eap::get_fvalue(c_temp_factor); 
		m_iterations_per_temperature_change = eap::get_fvalue(c_iterations_per_temp_change); 
		m_convergence_factor = eap::get_fvalue(c_convergence_factor); 
        m_converged_iterations = m_iterations * m_convergence_factor;
		std::cout<<"Completed SA parameter setup"<<std::endl;
	}
	catch (const eap::InvalidStateException &e)
	{
		std::cerr<<e.what()<<"\n";
	}
}

/**
* @desc Implements logic for SA runs
*/
void sa::run()
{
}

void sa::run_simulation(unsigned int id)
{
}

sa::~sa(void)
{
}
