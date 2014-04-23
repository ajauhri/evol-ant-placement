#include<cmath>
#include<exhaust.hpp>
#include<lua_cmds.hpp>
#include<eap_resources.hpp>

exhaust::exhaust(std::string lua_file) : algorithm(lua_file)
{
}

void exhaust::setup_algo_params()
{
    try
    {
        algorithm::setup_algo_params();
        m_nec_input = boost::format(eap::run_directory + "ind%09d");
    }
    catch (...)
    {
        throw;
    }
}

void exhaust::run(unsigned int run_id)
{
    try
    {
        std::vector<position_ptr> placements;
        std::cout<<"***creating individuals\n";
        recur_placements(placements, 0);
        std::cout<<"pop size"<<m_pop.size()<<"\n";
        evaluate();

        std::sort(m_pop.begin(), m_pop.end(), eap::gain_fitness_sort);
        m_max_gain = m_pop.back()->m_gain_fitness;
        
        std::sort(m_pop.begin(), m_pop.end(), eap::coupling_fitness_sort);
        m_min_coup = m_pop.front()->m_coupling_fitness;
        m_max_coup = m_pop.back()->m_coupling_fitness + std::abs(m_min_coup);

        for (individual_ptr i_ind : m_pop)
        {
            i_ind->m_coupling_fitness += std::abs(m_min_coup);
            if (i_ind->m_coupling_fitness < 0)
                throw eap::InvalidStateException("Invalid coupling calculation\n");
	        i_ind->m_coupling_fitness /= m_max_coup;
            i_ind->m_gain_fitness /= m_max_gain;
            i_ind->m_fitness = cal_fitness(i_ind);
        }

        std::sort(m_pop.begin(), m_pop.end(), eap::fitness_sort);
        std::cout<<"best "<<m_pop[0]->m_fitness<<"\n";
        save_norm(eap::run_directory);
        save_population(m_pop, run_id, 0);
        save_best_nec(m_pop[0], run_id, 0);
    }
    catch (...)
    {
        throw;
    }

}

void exhaust::evaluate()
{
    try
    {
        if (m_run_simulator)
            run_simulation(0); //argument doesn't signify anything here

        boost::format nec_output(eap::run_directory + "ind%09da%02d.out");
        for (unsigned int i=0; i<m_pop.size(); ++i)
        {
            for (unsigned int j=0; j<m_ant_configs.size(); ++j)
            {
                evaluation_ptr p_eval(new evaluation);
                m_pop[i]->m_evals.push_back(p_eval);
                unsigned int read = read_radiation(str(nec_output % i % j), p_eval);
                if (read != (num_polar() * m_step_freq))
                    throw eap::InvalidStateException("Problem with output in " + str(nec_output % i % j));
                m_pop[i]->m_one_ant_on_fitness.push_back(compare(m_free_inds[j]->m_evals[0], m_pop[i]->m_evals[j]));
                m_pop[i]->m_gain_fitness += m_pop[i]->m_one_ant_on_fitness[j];
            }
            m_pop[i]->m_coupling_fitness = read_coupling(str(nec_output % i % m_ant_configs.size()), m_ant_configs.size());
            if (m_pop[i]->m_coupling_fitness == 0.0f)
               throw eap::InvalidStateException("coupling bad\n");		
				
            m_pop[i]->m_fitness = cal_fitness(m_pop[i]);
        }
    }
    catch (...)
    {
        throw;
    }
}

void exhaust::run_simulation(unsigned int id)
{
    try
    {
        std::cout<<"***running simulation for exhaustive\n";
        std::string cmd("find " + eap::run_directory + " -iname \"*.nec\" | parallel -j+0 nec2++ -i {}");
        system(cmd.c_str());
        std::cout<<"***completed simulation for exhaustive\n";
    }
    catch (...)
    {
        throw;
    }
}

void exhaust::recur_placements(std::vector<position_ptr> &placements, unsigned int i)
{
    if(m_ant_configs.size() == placements.size())
    {
        m_pop.push_back(create_individual(str(m_nec_input % m_pop.size())+"a%02d.nec", placements));
        return;
    }

    for (unsigned int j=0; j<m_ant_configs[i]->m_positions.size(); j++)
    {
        if(!overlap(placements, m_ant_configs[i]->m_positions[j]))
        {
            placements.push_back(m_ant_configs[i]->m_positions[j]);
            recur_placements(placements, i+1);
            placements.pop_back();
        }
    }
}

exhaust::~exhaust(void)
{
}

