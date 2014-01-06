#include<iostream>
#include<vector>
#include<boost/algorithm/string.hpp>
#include<boost/format.hpp>
#include<boost/filesystem.hpp>
#include<boost/make_shared.hpp>
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
void ga::run(unsigned int run_id)
{
    try 
    {
        if (m_elitism > m_population_size)
            throw eap::InvalidStateException("GA:Elitism cannot be greater than population size");

        if (m_pop.size() > 1)
            m_pop.erase(m_pop.begin(), m_pop.end());

        if (m_pop.size() != 0)
            throw eap::InvalidStateException("GA: Population size should be zero");

        if (!boost::filesystem::create_directory(std::string(eap::run_directory+"gen0000")))
            throw eap::InvalidStateException("Problem creating directory");

        m_mutation += 0.05;

        boost::format nec_input(eap::run_directory + "gen%04d/ind%09d");
        for (unsigned int i_id=0; i_id<m_population_size; ++i_id)
        {
            std::vector<position_ptr> placements;
            for (ant_config_ptr i_ant : m_ant_configs)
            {
                int pos;
                do 
                {
                    pos = eap::rand(0, i_ant->m_positions.size()-1);
                } while (overlap(placements, i_ant->m_positions[pos]));
                placements.push_back(i_ant->m_positions[pos]);
            }
            m_pop.push_back(create_individual(str(nec_input % 0 % i_id) + "a%02d.nec", placements));
        }
        std::cout<<"***generation 0 created\n";
        evaluate_gen(0);

        for (unsigned int i=1; i<m_generations; ++i)
        {
            std::sort(m_pop.begin(), m_pop.end(), eap::fitness_sort);
            save_population(m_pop, run_id, i-1);
            save_best_nec(m_pop[0], run_id, i-1);

            std::cout<<"best "<<m_pop[0]->m_fitness<<"\n";
            select();
            create_generation(i);
            evaluate_gen(i);
        }

        std::sort(m_pop.begin(), m_pop.end(), eap::fitness_sort);
        std::cout<<"best "<<m_pop[0]->m_fitness<<"\n";
        save_population(m_pop, run_id, m_generations - 1);
        save_best_nec(m_pop[0], run_id, m_generations - 1);
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

void ga::evaluate_gen(unsigned int gen_id) 
{
    try
    {
        run_simulation(gen_id);
        boost::format nec_output(eap::run_directory + "gen%04d/ind%09da%02d.out");
        for (unsigned int i_pop=0; i_pop<m_pop.size(); ++i_pop)
        {
            for (unsigned int i_ant=0; i_ant<m_ant_configs.size(); ++i_ant)
            {
                evaluation_ptr p_eval(new evaluation);
                m_pop[i_pop]->m_evals.push_back(p_eval);
                unsigned int read = read_radiation(str(nec_output % gen_id % i_pop % i_ant), p_eval);
                if (read != (num_polar() * m_step_freq))
                    throw eap::InvalidStateException("Problem with output in " + str(nec_output % gen_id % i_pop % i_ant));
                m_pop[i_pop]->m_one_ant_on_fitness.push_back(compare(m_free_inds[i_ant]->m_evals[0], m_pop[i_pop]->m_evals[i_ant]));
                m_pop[i_pop]->m_gain_fitness += m_pop[i_pop]->m_one_ant_on_fitness[i_ant];
            }
            m_pop[i_pop]->m_gain_fitness /= m_max_gain;
            m_pop[i_pop]->m_coupling_fitness = read_coupling(str(nec_output % gen_id % i_pop % m_ant_configs.size()), m_ant_configs.size());
            m_pop[i_pop]->m_coupling_fitness += std::abs(m_min_coup);
            m_pop[i_pop]->m_coupling_fitness /= m_max_coup;
            m_pop[i_pop]->m_fitness = cal_fitness(m_pop[i_pop]);
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
            new_pop.push_back(boost::make_shared<individual>(*m_pop[i]));
        }

        // pick individuals in pairs
        for (unsigned int i = m_elitism; i < m_population_size; i+=2)
        {
            individual_ptr parent1 = m_pop[eap::rand(0, m_population_size-1)];
            individual_ptr parent2 = tour();
            std::vector<individual_ptr> offsprings;

            if (eap::rand01() < m_recombination)
            {
                offsprings = breed(parent1, parent2);
            }
            else
            {
                offsprings.push_back(boost::make_shared<individual>(*parent1));
                offsprings.push_back(boost::make_shared<individual>(*parent2));
            }
            //offsprings[0]->m_positions = mutate_pos(offsprings[0]->m_positions);
            //offsprings[0]->m_positions = mutate_pos(offsprings[0]->m_positions);

            new_pop.push_back(offsprings[0]);
            new_pop.push_back(offsprings[1]);
        }

        // pick m individuals from population and mutate one bit
        for (unsigned int i=0; i<(m_mutation*m_population_size); ++i)
        {
            int ind_id = eap::rand(0, m_population_size-1);
            simple_mutation(new_pop[ind_id]);
        }
 

        if (new_pop.size() != m_population_size) throw eap::InvalidStateException("GA: population size don't match");
        std::cout<<"***done with creating next generation\n";
        m_pop = new_pop;
    }
    catch (...)
    {
        throw;
    }
}

void ga::run_simulation(unsigned int gen_id)
{
    try
    {
        boost::format formatter("ls " + eap::run_directory + "gen%04d/*.nec | parallel -j+0 nec2++ -i {}");
        std::cout<<"***running simulation for generation "<<gen_id<<"\n";
        system(str(formatter % gen_id).c_str());
        std::cout<<"***completed simulation for generation "<<gen_id<<"\n";
    }
    catch (...)
    {
        throw;
    }
}

/**
 * @desc Tours a subset of the population `n` times, and selects the best fit indivudal 
 */
individual_ptr ga::tour()
{
    individual_ptr best;
    for (unsigned int i = 0; i < m_tournament_size; i++)
    {
        unsigned r_index = eap::rand(m_elitism, m_population_size-1);
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
