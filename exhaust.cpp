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

void exhaust::run()
{
    try
    {
        std::vector<position_ptr> placements;
        std::cout<<"***creating individuals\n";
        recur_placements(placements, 0);
        evaluate();
        std::sort(m_pop.begin(), m_pop.end(), eap::gain_fitness_sort);
        m_max_gain_fitness = m_pop.back()->m_gain_fitness;
        std::sort(m_pop.begin(), m_pop.end(), eap::coupling_fitness_sort);
        m_max_coup_fitness = m_pop.back()->m_gain_fitness;

        for (individual_ptr i_ind : m_pop)
        {
            i_ind->m_coupling_fitness /= m_max_coup_fitness;
            i_ind->m_gain_fitness /= m_max_gain_fitness;
            i_ind->m_fitness = cal_fitness(i_ind);
        }

        std::sort(m_pop.begin(), m_pop.end(), eap::fitness_sort);
        std::cout<<"best "<<m_pop[0]->m_fitness<<"\n";
        save_population(eap::run_directory, m_pop);
        save_best_nec(eap::run_directory, m_pop[0]);
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
        std::string cmd("ls " + eap::run_directory + "*.nec | parallel -j+0 ./nec2++.exe -i {}");
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

