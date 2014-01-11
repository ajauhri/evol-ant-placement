#include<es.hpp>
#include<eap_resources.hpp>
#include<lua_cmds.hpp>
#include<iostream>
#include<boost/filesystem.hpp>
#include<boost/format.hpp>
// Classical ES - \mu individuals are mutated to produce lambda new solutions and the algorithm chooses \mu + \lambda solutions to be the first mu for next generation
//
// http://stackoverflow.com/questions/12776719/is-cma-es-mu-lambda-or-mu-lambda
namespace 
{
    const std::string c_mu = "mu";
    const std::string c_lambda = "lambda";
    const std::string c_generations = "generations";
}


es::es(std::string lua_file) : algorithm(lua_file), m_mulambda_factor(0.0f)
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
        this->m_mu = eap::get_fvalue(c_mu); 
        this->m_lambda = eap::get_fvalue(c_lambda); 
        this->m_generations = eap::get_fvalue(c_generations);
        this->m_mulambda_factor = (unsigned int) floor((m_lambda - m_mu )/m_mu);
        std::cout<<"Completed ES parameter setup"<<std::endl;
    }
    catch (const eap::InvalidStateException &e)
    {
        std::cerr<<e.what()<<"\n";
    }
}

/**
 * @desc Implements logic for ES runs
 */
void es::run(unsigned int run_id)
{
    try
    {
        if (m_pop.size() > 1)
            m_pop.erase(m_pop.begin(), m_pop.end());

        if (m_pop.size() != 0)
            throw eap::InvalidStateException("ES: Population size sould be zero");

        boost::filesystem::create_directory(std::string(eap::run_directory+"gen0000"));
        boost::format nec_input(eap::run_directory + "gen%04d/ind%09d");

        for (unsigned int i_id=0; i_id<m_mu; ++i_id)
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

        for (unsigned int i=0; i<m_generations; ++i)
        {
            create_pop(i);
            evaluate_gen(i);
            std::sort(m_pop.begin(), m_pop.end(), eap::fitness_sort);
            save_population(m_pop, run_id, i);
            save_best_nec(m_pop[0], run_id, i);
            std::cout<<"best "<<m_pop[0]->m_fitness<<"\n";
            survivor_selection();
        }
    }

    catch (...)
    {
        throw;
    }
}

// extends population size to lambda using mutations based on (lamda/mu) factor
void es::create_pop(unsigned int gen)
{
    try
    {
        boost::format gen_dir(eap::run_directory+"gen%04d");
        boost::format input_path(eap::run_directory + "gen%04d/ind%09d");
        boost::filesystem::create_directory(str(gen_dir % gen));
        std::vector<individual_ptr> new_pop;

        if (gen != 0)
        {
            for (unsigned int i=0; i<m_mu; ++i)
            {
                new_pop.push_back(create_individual(str(input_path % gen % i) + "a%02d.nec", m_pop[i]->m_positions));
            }
        }
        else if (gen == 0)
        {
            for (unsigned int i=0; i<m_mu; ++i)
            {
                new_pop.push_back(m_pop[i]);
            }
        }

        for (unsigned int i=0; i<m_mu; ++i)
        {
            unsigned int counter = 0;
            while (counter<m_mulambda_factor)
            {
                int id = m_mu + i*m_mulambda_factor + counter;
                std::vector<position_ptr> placements = mutate_pos_once(m_pop[i]->m_positions);
                new_pop.push_back(create_individual(str(input_path % gen % id) + "a%02d.nec", placements));
                counter++;
            }
        }
        m_pop = new_pop;
    }
    catch (...)
    {
        throw;
    }
}

// (\mu + \lambda) selection
void es::survivor_selection()
{
    try
    {
        m_pop.erase(m_pop.begin() + m_mu, m_pop.end());
        if (m_pop.size() != m_mu)
            throw eap::InvalidStateException("population size does not equal to mu\n");
    }
    catch (...)
    {
        throw;
    }
}

void es::evaluate_gen(unsigned int gen_id)
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
            //normalize gain fitness
            m_pop[i_pop]->m_gain_fitness /= m_max_gain;
            m_pop[i_pop]->m_coupling_fitness = read_coupling(str(nec_output % gen_id % i_pop % m_ant_configs.size()), m_ant_configs.size());

            //normalize coupling fitness
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

void es::run_simulation(unsigned int gen_id)
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

es::~es(void)
{
}

