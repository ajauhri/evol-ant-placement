#include<iostream>
#include<vector>
#include<boost/algorithm/string.hpp>
#include<boost/format.hpp>
#include<boost/filesystem.hpp>
#include<ga.hpp>
#include<eap_resources.hpp>
#include<lua_cmds.hpp>
#include<sstream>

namespace
{
	char const *population_size_s = "pop";
	char const *generations_s = "generations";
	char const *tournament_size_s = "tournament_size";
	char const *elitism_s = "elitism";
	char const *recombination_s = "recombination";
}


ga::ga(std::string lua_file) : super(lua_file)
{ }

/**
* @desc Loads parameters for simple genetic algorithm  
*/
void ga::setup_algo_params()
{
	try 
	{
		algorithm::setup_algo_params();
		this->population_size = eap::get_fvalue(population_size_s);
		this->generations = eap::get_fvalue(generations_s);
		this->tournament_size = eap::get_fvalue(tournament_size_s);
		this->elitism = eap::get_fvalue(elitism_s);
		this->recombination = eap::get_fvalue(recombination_s);
		std::cout<<"***completed GA parameter setup \n";
	}
	catch (const eap::InvalidStateException &e)
	{
		std::cerr<<e.what()<<"\n";
	}
}

/**
* @desc Implements logic for GA runs
*/
void ga::run()
{
	algorithm::setup_run_context();

	if (elitism > population_size)
		throw eap::InvalidStateException("Elitism cannot be greater than population size");

	boost::filesystem::create_directory(std::string(eap::run_directory+"gen0000"));
   /* 
	for (unsigned int ind_id=0; ind_id<this->population_size; ++ind_id)
	{
        boost::format formatter(eap::run_directory + "gen0000/ind%09da%2d.nec");
        for (unsigned int ant_id=0; ant_id<this->ant_configs.size(); ++ant_id)
        {



		sprintf(file_path, "./%s/config%04d.xml", folder, config_id);
		pop.push_back(setup_individual());
		write_to_file(pop[config_id], file_path);
        }
	}
	std::cout<<"Generation 0 created"<<std::endl;
	for (unsigned int generation=1; generation<this->generations; generation++)
	{
		run_simulation();
		select();
		sprintf(folder, "Runs/GEN%04d", generation);
		boost::filesystem::create_directory(folder);
		
		for (unsigned int config_id=0; config_id<this->population_size; config_id++)
		{
			char file_path[500];
			sprintf(file_path, "./%s/config%04d.xml", folder, config_id);
			write_to_file(pop.at(config_id), file_path);
		}
	}
    */

}

#if 0
/**
* @desc Implements stochastic operators viz. recombination and mutation on the population
*/
void ga::select()
{
	std::vector<individual_ptr> new_pop;
	for (unsigned int i=0; i<elitism; i++)
	{	
		new_pop.push_back(pop[i]);
		for (unsigned j=0; j<ant_configs.size(); j++)
					std::cout<<new_pop.at(i)->ant_configs.at(j)->positions.back()->mount_object<<std::endl;
	}

	for (unsigned int i = elitism; i < population_size; i+=2)
	{
		//TODO: Check whether tour() should be performed even without recombination. 
		individual_ptr parent_1 = tour();
		individual_ptr parent_2 = tour();

		if (aapot_resources::randf(0, 1.0) < recombination_probability)
		{
			std::vector<individual_ptr> ind_vector = algorithm::breed(parent_1, parent_2);
			new_pop.push_back(ind_vector[0]);
			new_pop.push_back(ind_vector[1]);
			algorithm::simple_mutation(ind_vector[0]);
			algorithm::simple_mutation(ind_vector[1]);
		}
		else
		{
			new_pop.push_back(parent_1);
			new_pop.push_back(parent_2);
		}
	}
	pop = new_pop;
}

/**
* @desc Tours the population and selects the best fit indivudal 
*/
individual_ptr ga::tour()
{
	individual_ptr best;
	for (unsigned int i = 0; i < tournament_size; i++)
	{
		unsigned r_index = rand_integer(0, population_size);
		if (!best || pop[r_index]->fitness < best->fitness) 
			best = pop[r_index];
	}
	
	return best;
}
#endif

ga::~ga(void)
{
}
