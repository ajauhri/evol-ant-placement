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


ga::ga(std::string lua_file) : algorithm(lua_file)
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
    catch (...)
    {
        throw;
    }
}

/**
 * @desc Implements logic for GA runs
 */
void ga::run()
{
    try 
    {
        if (elitism > population_size)
            throw eap::InvalidStateException("Elitism cannot be greater than population size");

        boost::filesystem::create_directory(std::string(eap::run_directory+"gen0000"));

        boost::format nec_input(eap::run_directory + "gen%04d/ind%09d");
        for (unsigned int ind_id=0; ind_id<population_size; ++ind_id)
        {
            std::vector<position_ptr> placements;
            for (ant_config_ptr ant : ant_configs)
            {
                int pos = eap::rand(0, ant->positions.size()-1);
                placements.push_back(ant->positions[pos]);
            }
            pop.push_back(create_individual(str(nec_input % 0 % ind_id)+"a%02d.nec", placements));
        }
        std::cout<<"***generation 0 created\n";
        evaluate_gen(0);

        for (unsigned int i=1; i<generations; ++i)
        {
            std::sort(pop.begin(), pop.end(), eap::fitness_sort);
            std::cout<<"best "<<pop[0]->fitness<<"\n";
            select();
            create_generation(i);
            evaluate_gen(i);
        }
        std::sort(pop.begin(), pop.end(), eap::fitness_sort);
        std::cout<<"best "<<pop[0]->fitness<<"\n";
    }
    catch (...)
    {
        throw;
    }
}

void ga::create_generation(unsigned int gen)
{
    try
    {
        boost::format gen_dir(eap::run_directory+"gen%04d");
        boost::format input_path(eap::run_directory + "gen%04d/ind%09d");
        boost::filesystem::create_directory(str(gen_dir % gen));

        for (unsigned int ind_id=0; ind_id<population_size; ++ind_id)
        {
            pop[ind_id] = create_individual(str(input_path % gen % ind_id) + "a%02d.nec", pop[ind_id]->positions);
        }

        if (pop.size() != population_size) throw eap::InvalidStateException("pop vector size " + std::to_string(pop.size()) + " and " + std::to_string(population_size) + " specified in lua");
    }
    catch (...)
    {
        throw;
    }
}

void ga::evaluate_gen(unsigned int id)
{
    try
    {
        run_simulation(id);
        boost::format nec_output(eap::run_directory + "gen%04d/ind%09da%02d.out");
        for (unsigned int i=0; i<pop.size(); ++i)
        {
            for (unsigned int j=0; j<ant_configs.size(); ++j)
            {
                evaluation_ptr eval(new evaluation);
                pop[i]->evals.push_back(eval);
                unsigned int read = read_nou(str(nec_output % id % i % j), eval);
                if (read != (num_polar() * step_freq))
                    throw eap::InvalidStateException("Problem with output in " + str(nec_output % id % i % j));
                pop[i]->one_ant_on_fitness.push_back(compare(free_inds[j]->evals[0], pop[i]->evals[j]));
                pop[i]->fitness += pop[i]->one_ant_on_fitness[j];
            }
        }
    }
    catch (...)
    {
        throw;
    }
}


/**
 * @desc Implements stochastic operators viz. recombination and mutation on the population
 */
void ga::select()
{
    std::vector<individual_ptr> new_pop;
    try
    {

        for (unsigned int i=0; i<elitism; i++)
        {	
            new_pop.push_back(pop[i]);
        }
        
        // pick individuals in pairs
        for (unsigned int i = elitism; i < population_size; i+=2)
        {
            individual_ptr parent1 = tour();
            individual_ptr parent2 = tour();

            if (eap::rand01() < recombination)
            {
                std::vector<individual_ptr> children = breed(parent1, parent2);
                new_pop.push_back(children[0]);
                new_pop.push_back(children[1]);
            }
            else
            {
                new_pop.push_back(parent1);
                new_pop.push_back(parent2);
            }
        }

        // pick m individuals from population and mutate one bit of theirs
        for (unsigned int i=0; i<(mutation*population_size); ++i)
        {
            int ind_id = eap::rand(0, population_size-1);
            simple_mutation(new_pop[ind_id]);
        }
        
        if (new_pop.size() != population_size) throw eap::InvalidStateException("population size don't match");
        std::cout<<"***done with creating next generation\n";
        pop.swap(new_pop);
    }
    catch (...)
    {
        throw;
    }
}

void ga::run_simulation(unsigned int id)
{
    try
    {
        boost::format formatter("ls " + eap::run_directory + "gen%04d/*.nec | parallel -j+0 ./nec2++.exe -i {}");
        std::cout<<"***running simulation for generation "<<id<<"\n";
        system(str(formatter % id).c_str());
        std::cout<<"***completed simulation for generation "<<id<<"\n";
    }
    catch (...)
    {
        throw;
    }
}

/**
 * @desc Tours the population and selects the best fit indivudal 
 */
individual_ptr ga::tour()
{
    individual_ptr best;
    for (unsigned int i = 0; i < tournament_size; i++)
    {
        unsigned r_index = eap::rand(0, population_size-1);
        if (!best || pop[r_index]->fitness < best->fitness) 
            best = pop[r_index];
    }

    return best;
}

ga::~ga(void)
{
    pop.clear();
    pop.shrink_to_fit();
}
