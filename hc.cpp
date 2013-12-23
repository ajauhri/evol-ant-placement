#include<iostream>
#include<hc.hpp>
#include<eap_resources.hpp>
#include<lua_cmds.hpp>
#include<boost/format.hpp>

namespace 
{
    const std::string c_iterations = "iterations";
    const std::string c_convergence_factor = "convergence_factor_s";
}

hc::hc(std::string lua_file) : algorithm(lua_file)
{
    m_convergence_factor = 0.0f;
    m_best_fitness = 0.0f;
    m_converged_iterations = 0.0f;
}


/**
 * @desc Loads parameters for the hill climber algorithm  
 */
void hc::setup_algo_params()
{
    try
    {
        algorithm::setup_algo_params();
        m_iterations = eap::get_fvalue(c_iterations);
        m_convergence_factor = eap::get_fvalue(c_convergence_factor);
        m_converged_iterations = m_iterations * m_convergence_factor;
        std::cout<<"Completed HC parameter setup"<<std::endl;
    }
    catch (const eap::InvalidStateException &e)
    {
        std::cerr<<e.what()<<"\n";
    }
}

/**
 * @desc Implements logic for HC runs
 */
void hc::run()
{
    try
    {
        std::vector<position_ptr> placements;
        boost::format nec_input(eap::run_directory + "iter%09d");
        int q = 0; // successive state with best solution

        for (ant_config_ptr i_ant : m_ant_configs)
        {
            int pos = eap::rand(0, i_ant->m_positions.size() - 1);
            placements.push_back(i_ant->m_positions[pos]);
        }

        m_p_parent = create_individual(str(nec_input % 0) + "a%02d.nec", placements);
        evaluate(0, m_p_parent);
        m_best_fitness = m_p_parent->m_fitness;

        for (unsigned int i=1; i<m_iterations; ++i)
        {
            std::vector<position_ptr> placements = mutate_pos(m_p_parent->m_positions);
            individual_ptr p_child = create_individual(str(nec_input % i) + "a%02d.nec", placements);
            evaluate(i, p_child);

            if (p_child->m_fitness < m_p_parent->m_fitness)
            {
                m_best_fitness = p_child->m_fitness;
                swap(m_p_parent, p_child);
                std::cout<<"***iter="<<i<<", best ind "<<m_p_parent->m_fitness<<"\n";
            }

            if (m_p_parent->m_fitness >= m_best_fitness)
                q++;
            else 
                q=0;

            if (q > m_converged_iterations)
            {
                //change mutation probability
                q = 0;
            }
        }
    }
    catch (...)
    {
        throw;
    }
}

std::vector<position_ptr> hc::mutate_pos(std::vector<position_ptr> &orig_placements)
{
    try
    {
        std::vector<position_ptr> placements;
        for (unsigned int i_ant=0; i_ant<orig_placements.size(); ++i_ant)
        {
            if (eap::rand01() < m_mutation)
            {
                int pos = eap::rand(0, m_ant_configs[i_ant]->m_positions.size() - 1);
                placements.push_back(m_ant_configs[i_ant]->m_positions[pos]);
            }
            placements.push_back(orig_placements[i_ant]);
        }
        return placements;
    }
    catch (...)
    {
        throw;
    }
}


void hc::evaluate(unsigned int id, individual_ptr &p_ind)
{
    try
    {
        run_simulation(id);
        boost::format nec_output(eap::run_directory + "iter%09da%2d.out");
        for (unsigned int i_ant=0; i_ant<m_ant_configs.size(); ++i_ant)
        {
            evaluation_ptr p_eval(new evaluation);
            p_ind->m_evals.push_back(p_eval);
            unsigned int read = read_radiation(str(nec_output % id % i_ant), p_eval);
            if (read != (num_polar() * m_step_freq))
                throw eap::InvalidStateException("HC:Problem with output in " + str(nec_output % id % i_ant));
            p_ind->m_one_ant_on_fitness.push_back(compare(m_free_inds[i_ant]->m_evals[0], p_ind->m_evals[i_ant]));
            p_ind->m_gain_fitness += p_ind->m_one_ant_on_fitness[i_ant];
        }
        p_ind->m_gain_fitness /= m_max_gain;
        p_ind->m_coupling_fitness = read_coupling(str(nec_output % id % m_ant_configs.size()), m_ant_configs.size());
        p_ind->m_fitness = cal_fitness(p_ind);
    }
    catch (...)
    {
        throw;
    }
}

void hc::run_simulation(unsigned int id)
{
    try
    {
        boost::format formatter("ls " + eap::run_directory + "iter%09da*.nec | parallel -j+0 ./nec2++.exe -i {}");
        system(str(formatter % id).c_str());
        std::cout<<"***completed simulation for iteration "<<id<<"\n";
    }
    catch (...)
    {
        throw;
    }
}

hc::~hc(void)
{
}
