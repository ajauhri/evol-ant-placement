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
    const std::string c_population_size = "pop";
    const std::string c_generations = "generations";
    const std::string c_tournament_size = "tournament_size";
    const std::string c_elitism = "elitism";
    const std::string c_recombination = "recombination";
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
        m_population_size = eap::get_fvalue(c_population_size);
        m_generations = eap::get_fvalue(c_generations);
        m_tournament_size = eap::get_fvalue(c_tournament_size);
        m_elitism = eap::get_fvalue(c_elitism);
        m_recombination = eap::get_fvalue(c_recombination);
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
        if (m_elitism > m_population_size)
            throw eap::InvalidStateException("Elitism cannot be greater than population size");

        boost::filesystem::create_directory(std::string(eap::run_directory+"gen0000"));

        boost::format nec_input(eap::run_directory + "gen%04d/ind%09d");
        for (unsigned int i_id=0; i_id<m_population_size; ++i_id)
        {
            std::vector<position_ptr> placements;
            for (ant_config_ptr ant : m_ant_configs)
            {
                int pos = eap::rand(0, ant->m_positions.size()-1);
                placements.push_back(ant->m_positions[pos]);
            }
            m_pop.push_back(create_individual(str(nec_input % 0 % i_id)+"a%02d.nec", placements));
        }
        std::cout<<"***generation 0 created\n";
        evaluate_gen(0);

        for (unsigned int i=1; i<m_generations; ++i)
        {
            std::sort(m_pop.begin(), m_pop.end(), eap::fitness_sort);
            std::cout<<"best "<<m_pop[0]->m_fitness<<"\n";
            select();
            create_generation(i);
            evaluate_gen(i);
        }
        std::sort(m_pop.begin(), m_pop.end(), eap::fitness_sort);
        std::cout<<"best "<<m_pop[0]->m_fitness<<"\n";
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

        for (unsigned int i_id=0; i_id<m_population_size; ++i_id)
        {
            m_pop[i_id] = create_individual(str(input_path % gen % i_id) + "a%02d.nec", m_pop[i_id]->m_positions);
        }

        if (m_pop.size() != m_population_size) throw eap::InvalidStateException("pop vector size " + std::to_string(m_pop.size()) + " and " + std::to_string(m_population_size) + " specified in lua");
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
        for (unsigned int i=0; i<m_pop.size(); ++i)
        {
            for (unsigned int j=0; j<m_ant_configs.size(); ++j)
            {
                evaluation_ptr eval(new evaluation);
                m_pop[i]->m_evals.push_back(eval);
                unsigned int read = read_nou(str(nec_output % id % i % j), eval);
                if (read != (num_polar() * m_step_freq))
                    throw eap::InvalidStateException("Problem with output in " + str(nec_output % id % i % j));
                m_pop[i]->m_one_ant_on_fitness.push_back(compare(m_free_inds[j]->m_evals[0], m_pop[i]->m_evals[j]));
                m_pop[i]->m_fitness += m_pop[i]->m_one_ant_on_fitness[j];
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
        for (unsigned int i=0; i<m_elitism; i++)
        {	
            new_pop.push_back(m_pop[i]);
        }
        
        // pick individuals in pairs
        for (unsigned int i = m_elitism; i < m_population_size; i+=2)
        {
            individual_ptr parent1 = tour();
            individual_ptr parent2 = tour();

            if (eap::rand01() < m_recombination)
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
        for (unsigned int i=0; i<(m_mutation*m_population_size); ++i)
        {
            int ind_id = eap::rand(0, m_population_size-1);
            simple_mutation(new_pop[ind_id]);
        }
        
        if (new_pop.size() != m_population_size) throw eap::InvalidStateException("population size don't match");
        std::cout<<"***done with creating next generation\n";
        m_pop.swap(new_pop);
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
    for (unsigned int i = 0; i < m_tournament_size; i++)
    {
        unsigned r_index = eap::rand(0, m_population_size-1);
        if (!best || m_pop[r_index]->m_fitness < best->m_fitness) 
            best = m_pop[r_index];
    }
    return best;
}

ga::~ga(void)
{
    m_pop.clear();
    m_pop.shrink_to_fit();
}
