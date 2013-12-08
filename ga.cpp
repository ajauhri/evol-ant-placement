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
    if (elitism > population_size)
        throw eap::InvalidStateException("Elitism cannot be greater than population size");

    boost::filesystem::create_directory(std::string(eap::run_directory+"gen0000"));

    boost::format nec_input(eap::run_directory + "gen%04d/ind%09da%02d.nec");
    boost::format nec_output(eap::run_directory + "gen%04d/ind%09da%02d.out");
    for (unsigned int ind_id=0; ind_id<this->population_size; ++ind_id)
    {
        individual_ptr ind(new individual);
        std::vector<unsigned int> placements;
        for (ant_config_ptr ant : this->ant_configs)
            placements.push_back(eap::rand(0, ant->positions.size()-1));

        for (unsigned int j=0; j<this->ant_configs.size(); ++j)
        {
            ind->positions.push_back(this->ant_configs[j]->positions[placements[j]]);
            std::ofstream outfile(str(nec_input % 0 % ind_id % j));
            write_platform(outfile);

            int count = this->platform->nec_wires.size();
            int excitation_id;
            for (unsigned int k=0; k<this->ant_configs.size(); ++k)
            {
                if (k==j)
                    excitation_id = count+1;
                write_ant(outfile, this->ant_configs[k], placements[k], count+1);
                count += this->ant_configs[k]->wires.size();
            }
            write_excitation(outfile, excitation_id);
            outfile.close();
        }
        pop.push_back(ind);
    }
    std::cout<<"***generation 0 created\n";

    try 
    { 
        for (unsigned int gen=0; gen<1; ++gen)
        {
            run_simulation(gen);
            for (unsigned int i=0; i<this->pop.size(); ++i)
            {
                for (unsigned int j=0; j<this->ant_configs.size(); ++j)
                {
                    evaluation_ptr eval(new evaluation);
                    pop[i]->evals.push_back(eval);
                    unsigned int read = read_nou(str(nec_output % gen % i % j), eval);
                    if (read != (num_polar() * step_freq))
                        throw eap::InvalidStateException("Problem with output in " + str(nec_output % gen % i % j));
                    pop[i]->one_ant_on_fitness.push_back(compare(free_inds[j]->evals[0], pop[i]->evals[j]));
                    pop[i]->fitness += pop[i]->one_ant_on_fitness[j];
                }
                std::cout<<pop[i]->fitness<<"\n";
            }
            std::sort(pop.begin(), pop.end(), eap::fitness_sort);
            std::cout<<"best "<<pop[0]->fitness<<"\n";


            /*
               select();
               sprintf(folder, "Runs/GEN%04d", generation);
               boost::filesystem::create_directory(folder);

               for (unsigned int config_id=0; config_id<this->population_size; config_id++)
               {
               char file_path[500];
               sprintf(file_path, "./%s/config%04d.xml", folder, config_id);
               write_to_file(pop.at(config_id), file_path);
               }*/
        }
    }
    catch (...)
    {
        throw;
    }
}


void ga::run_simulation(unsigned int id)
{
    boost::format formatter("ls " + eap::run_directory + "gen%04d/*.nec | parallel -j+0 ./nec2++.exe -i {}");
    std::cout<<"***running nec for id "<<id<<"\n";
    system(str(formatter % id).c_str());
}

/**
 * @desc Implements stochastic operators viz. recombination and mutation on the population
 */
void ga::select()
{
    std::vector<individual_ptr> new_pop;
    for (unsigned int i=0; i<elitism; i++)
    {	
        new_pop.push_back(pop[i]);
    }

    for (unsigned int i = elitism; i < population_size; i+=2)
    {
        //TODO: Check whether tour() should be performed even without recombination. 
        individual_ptr parent1 = tour();
        individual_ptr parent2 = tour();

        if (eap::rand01() < recombination)
        {
            std::vector<individual_ptr> children = breed(parent1, parent2);
            new_pop.push_back(children[0]);
            new_pop.push_back(children[1]);
            simple_mutation(children[0]);
            simple_mutation(children[1]);
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
