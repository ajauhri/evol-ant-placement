#include<sa.hpp>
#include<eap_resources.hpp>
#include<lua_cmds.hpp>
#include<iostream>
#include<boost/format.hpp>
#include<boost/filesystem.hpp>
#include<math.h>


namespace
{
    const std::string c_iterations = "iterations";
    const std::string c_cooling_factor = "cooling_factor";
    const std::string c_convergence_factor = "convergence_factor";
    const std::string c_temp_pop_factor = "temp_pop_factor";
    const std::string c_temp_error = "temp_error";
    const std::string c_accept_prob = "accept_prob";
}

sa::sa(std::string lua_file) : algorithm(lua_file)
{
    m_init_temp = 0.0f;
    m_cooling_factor = 0.0f;
    m_convergence_factor = 0.0f;
    m_converged_iterations = 0.0f;
    m_best_fitness = 0.0f;
    m_temp_pop_factor = 0.0f;

    // for computing initial temperature
    m_accept_prob = 0.0f;
    m_e = 0.0f;
    m_p = 100;

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
        m_init_temp = 100; // to be calculated using http://cs.stackexchange.com/questions/11126/initial-temperature-in-simulated-annealing-algorithm 
        m_cooling_factor = eap::get_fvalue(c_cooling_factor); 
        m_convergence_factor = eap::get_fvalue(c_convergence_factor);
        m_temp_pop_factor = eap::get_fvalue(c_temp_pop_factor);
        m_e = eap::get_fvalue(c_temp_error);
        m_accept_prob = eap::get_fvalue(c_accept_prob);
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
void sa::run(unsigned int run_id)
{
    std::ofstream outfile;
    try
    {
        if (run_id == 0) 
        {
            //compute_temp();
            m_init_temp = 0.27;
            std::cout<<"***init computed temperature = "<<m_init_temp<<"\n";
        }
        std::vector<position_ptr> placements;
        boost::format nec_input(eap::run_directory + "iter%09d");
        outfile.open(eap::results_directory + boost::filesystem::basename(m_lua_file) + "_r" + std::to_string(run_id) + "_iters.csv");
        float temperature = m_init_temp;
        int q = 0; //successive state with best solution
        //p successive temperatures best solution holds

        for (ant_config_ptr i_ant : m_ant_configs)
        {
            int pos;
            do 
            {
                pos = eap::rand(0, i_ant->m_positions.size()-1);
            } while(overlap(placements, i_ant->m_positions[pos]));
            placements.push_back(i_ant->m_positions[pos]);
        }
        m_p_parent = create_individual(str(nec_input % 0) + "a%02d.nec", placements);
        evaluate(0, m_p_parent);
        m_best_fitness = m_p_parent->m_fitness;
        outfile << 0 << "," << m_p_parent->m_fitness << "," << m_p_parent->m_gain_fitness << "," << m_p_parent->m_coupling_fitness << ",";
        for (position_ptr p_pos : m_p_parent->m_positions)
            outfile << p_pos->m_x << "," << p_pos->m_y << "," << p_pos->m_z <<",";
        outfile << "\n";

        for (unsigned int i=1; i<m_iterations; ++i)
        {
            placements = mutate_pos_once(m_p_parent->m_positions);
            individual_ptr p_child = create_individual(str(nec_input % i) + "a%02d.nec", placements);
            evaluate(i, p_child);

            if (p_child->m_fitness < m_p_parent->m_fitness)
            {
                m_best_fitness = p_child->m_fitness;
                swap(m_p_parent, p_child);
                outfile << i << "," << m_p_parent->m_fitness << "," << m_p_parent->m_gain_fitness << "," << m_p_parent->m_coupling_fitness << ",";
                for (position_ptr p_pos : m_p_parent->m_positions)
                    outfile << p_pos->m_x << "," << p_pos->m_y << "," << p_pos->m_z <<",";
                outfile << "\n";

                std::cout<<"***fitness improved iter="<<i<<", best ind "<<m_p_parent->m_fitness<<"\n";

            }
            else if (p_child->m_fitness > m_p_parent->m_fitness)
            {
                float delta_fitness = (p_child->m_fitness - m_p_parent->m_fitness);
                if (eap::rand01() < exp((-1.0 * delta_fitness) / temperature))
                {
                    std::cout<<"SA GOT BAD " << exp((-1.0 * delta_fitness) / temperature)<<"\n";
                    swap(m_p_parent, p_child);
                    outfile << i << "," << m_p_parent->m_fitness << "," << m_p_parent->m_gain_fitness << "," << m_p_parent->m_coupling_fitness << ",";
                    for (position_ptr p_pos : m_p_parent->m_positions)
                        outfile << p_pos->m_x << "," << p_pos->m_y << "," << p_pos->m_z <<",";
                    outfile << "\n";
                    std::cout<<"***fitness got bad iter="<<i<<", best ind "<<m_p_parent->m_fitness<<"\n";
                    outfile.flush();
                }
            }
            temperature = m_cooling_factor * temperature;

            // for q iterations, the best solution didn't improve
            if (m_p_parent->m_fitness >= m_best_fitness)
                q++;
            else
                q = 0;

            if (q > m_converged_iterations)
                break;

            // remove all after an iteration
            boost::filesystem::path path_to_remove(eap::run_directory);
            for (boost::filesystem::directory_iterator end_dir_it, it(path_to_remove); it!=end_dir_it; ++it)
                remove_all(it->path());

        }
        outfile.close();
    }
    catch (...)
    {
        outfile.close();
        throw;
    }
}

void sa::evaluate(unsigned int id, individual_ptr &p_ind)
{
    try
    {
        //run_simulation(id);
        boost::format nec_output(eap::run_directory + "iter%09da%02d.out");
        std::ifstream infile;
        infile.open("tc1_ex.csv");
        /*
        for (unsigned int i_ant=0; i_ant<m_ant_configs.size(); ++i_ant)
        {
            evaluation_ptr p_eval(new evaluation);
            p_ind->m_evals.push_back(p_eval);
            unsigned int read = read_radiation(str(nec_output % id % i_ant), p_eval);
            if (read != (num_polar() * m_step_freq))
                throw eap::InvalidStateException("SA:Problem with output in " + str(nec_output % id % i_ant));
            p_ind->m_one_ant_on_fitness.push_back(compare(m_free_inds[i_ant]->m_evals[0], p_ind->m_evals[i_ant]));
            p_ind->m_gain_fitness += p_ind->m_one_ant_on_fitness[i_ant];
        }

        // normalize gain fitness
        p_ind->m_gain_fitness /= m_max_gain;
        p_ind->m_coupling_fitness = read_coupling(str(nec_output % id % m_ant_configs.size()), m_ant_configs.size());

        // normalize coupling fitness
        p_ind->m_coupling_fitness += std::abs(m_min_coup);
        p_ind->m_coupling_fitness /= m_max_coup;

        p_ind->m_fitness = cal_fitness(p_ind);
        */
        std::ostringstream pos_str;
        pos_str << ",";
        for (unsigned int i_ant=0; i_ant<m_ant_configs.size(); ++i_ant)
            pos_str << p_ind->m_positions[i_ant]->m_x << "," << p_ind->m_positions[i_ant]->m_y << "," << p_ind->m_positions[i_ant]->m_z << ",";
        std::string line;
        while (std::getline(infile, line) && line.find(pos_str.str()) == std::string::npos);
        std::vector<std::string> vals;
        split(line, ',', vals);
        p_ind->m_coupling_fitness = std::stof(vals[2]);
        p_ind->m_gain_fitness = std::stof(vals[1]);
        p_ind->m_fitness = std::stof(vals[0]);
        pos_str.clear();
        pos_str.str("");
        line.clear();
        infile.close();

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
        boost::format formatter("ls " + eap::run_directory + "iter%09da*.nec | parallel -j+0 nec2++ -i {}");
        system(str(formatter % id).c_str());
        std::cout<<"***completed simulation for iteration "<<id<<"\n";
    }
    catch (...)
    {
        throw;
    }
}

void sa::compute_temp()
{
    // populate m_S a.k.a. set for transistions s.t. the hypothesis after the tranisition is less fitter than the hypothesis before the transition
    std::cout<<"***computing initial temp\n";

    boost::format nec_input(eap::run_directory + "iter%09d");

    // calculate total possible permutations. Assumption - that none of the placements overlap
    float tot_size = 1; 
    unsigned int curr_size = 0;
    for (ant_config_ptr i_ant : m_ant_configs)
        tot_size *= i_ant->m_positions.size();

    while (curr_size <= m_temp_pop_factor * tot_size) 
    {
        transition_ptr p_s(new transition);
        individual_ptr p_min(new individual);
        individual_ptr p_max(new individual);
        std::vector<position_ptr> placements;

        for (ant_config_ptr i_ant : m_ant_configs)
        {
            int pos;
            do 
            {
                pos = eap::rand(0, i_ant->m_positions.size() - 1);
            } while (overlap(placements, i_ant->m_positions[pos]));
            placements.push_back(i_ant->m_positions[pos]);
        }
        p_min = create_individual(str(nec_input % curr_size) + "a%02d.nec", placements);
        evaluate(curr_size, p_min);

        placements = mutate_pos_once(p_min->m_positions);
        p_max = create_individual(str(nec_input % (curr_size+1)) + "a%02d.nec", placements);
        evaluate(curr_size + 1, p_max);

        if (p_max->m_fitness < p_min->m_fitness)
            p_min.swap(p_max);

        p_s->m_min = p_min;
        p_s->m_max = p_max;
        m_S.push_back(p_s);
        curr_size += 2;

        std::cout<<p_s->m_min->m_fitness<<" "<<p_s->m_max->m_fitness<<"\n";

        unsigned int overlaps = 0;
        for (position_ptr i_ant : p_max->m_positions)
        {
            if (overlap(p_min->m_positions, i_ant))
                overlaps++;
        }
        if (overlaps != m_ant_configs.size() - 1)
            throw eap::InvalidStateException("All except one antenna position should not overlap");
        // remove all after an iteration
        boost::filesystem::path path_to_remove(eap::run_directory);
        for (boost::filesystem::directory_iterator end_dir_it, it(path_to_remove); it!=end_dir_it; ++it)
            remove_all(it->path());
    }

    boost::filesystem::remove_all(eap::run_directory);
    boost::filesystem::create_directory(eap::run_directory);

    float num = 0.0f;
    float deno = 0.0f;
    while (1)
    {
        for (transition_ptr p_s : m_S)
        {
            num += exp(-(p_s->m_max->m_fitness) / m_init_temp);
            deno += exp(-(p_s->m_min->m_fitness) / m_init_temp);
        }
        std::cout<<"std::abs((num/den)-m_accept_prob)"<<std::abs((num/deno) - m_accept_prob)<<"\n";

        if (std::abs( (num/deno) - m_accept_prob) < m_e)
            break;
        else 
        {
            m_init_temp = m_init_temp * pow((log(num/deno) / log(m_accept_prob)), 1/m_p);
            std::cout<<"updating init temp= "<<m_init_temp<<"\n";
        }
    }
}

sa::~sa(void)
{
}
