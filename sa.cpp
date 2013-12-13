#include<sa.hpp>
#include<eap_resources.hpp>
#include<lua_cmds.hpp>
#include<iostream>
#include<limits>
#include<boost/format.hpp>


namespace
{
	const std::string c_iterations = "iterations";
	const std::string c_init_temp = "init_temp";
	const std::string c_temp_factor = "temp_factor";
	const std::string c_iterations_per_temp_change = "iterations_per_temp_change";
	const std::string c_convergence_factor = "convergence_factor";
}

sa::sa(std::string lua_file) : algorithm(lua_file), m_best_fitness(std::numeric_limits<float>::max())
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
        
        // m_converged_iterations is computed 
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
#if 0
    try
    {
        std::vector<position_ptr> placements;
        boost::format nec_input(eap::run_directory + "iter%09d");
        for (ant_configs_ptr i_ant : m_ant_configs)
        {
            int pos = eap::rand(0, i_ant->m_positions.size() - 1);
            placements.push_back(i_ant->m_positions[pos]);
        }
        m_p_parent = create_individual(str(nec_input % m_iter) + "a%02d.nec", placements);
        evaluate_iter(m_iter);
        
        m_best_fitness = m_p_parent->m_fitness; 
        m_iter++;

        for (unsigned int i=1; i<m_iterations; ++i)
        {
           std::vector<position_ptr> placements = mutate_pos(m_p_parent->positions);
           individual_ptr p_child = create_individual(str(nec_input % m_iter++) + "a%02d.nec", placements);
           evaluate_iter(m_iter);
           if (p_child->m_fitness < m_p_parent->m_fitness)
           {
               m_best_fitness = p_child->m_fitness;
               swap(m_p_parent, p_child);
           }
           else if (p_child->m_fitness > m_p_parent->m_fitness)
           {
               float delta_fitness = ((p_child->m_fitness - m_p_parent->m_fitness) / 100);
               float prob = exp((-1.0 * delta_fitness) / temperature);



           }


           

        }

    }
    catch (...)
    {
        throw;
    }
#endif
}

std::vector<position_ptr> sa::mutate_pos(std::vector<position_ptr> orig_placements)
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

void sa::evaluate_iter(unsigned int id)
{
    try
    {
        run_simulation(id);
        boost::format nec_output(eap::run_directory + "iter%09da%2d.out");
        for (unsigned int i_ant=0; i_ant<m_ant_configs.size(); ++i_ant)
        {
            evaluation_ptr p_eval(new evaluation);
            m_p_parent->m_evals.push_back(p_eval);
            unsigned int read = read_nou(str(nec_output % id % i_ant), p_eval);
            if (read != (num_polar() * m_step_freq))
                throw eap::InvalidStateException("Problem with output in " + str(nec_output % id % i_ant));
            m_p_parent->m_one_ant_on_fitness.push_back(compare(m_free_inds[i_ant]->m_evals[0], m_p_parent->m_evals[i_ant]));
            m_p_parent->m_fitness += m_p_parent->m_one_ant_on_fitness[i_ant];
        }
    }
    catch (...)
    {
        throw;
    }
}

void sa::run_simulation(unsigned int id)
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

void sa::save_best_nec()
{
}

sa::~sa(void)
{
}
