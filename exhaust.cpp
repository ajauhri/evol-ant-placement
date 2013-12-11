#include<exhaust.hpp>
#include<eap_resources.hpp>

exhaust::exhaust(std::string lua_file) : algorithm(lua_file)
{
}

void exhaust::setup_algo_params()
{
    try
    {
        nec_input = boost::format(eap::run_directory + "ind%09d");
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
        std::sort(pop.begin(), pop.end(), eap::fitness_sort);
        std::cout<<"best "<<pop[0]->fitness<<"\n";
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
        run_simulation(0); //argument doesn't signify anything here
        boost::format nec_output(eap::run_directory + "ind%09da%02d.out");
        for (unsigned int i=0; i<pop.size(); ++i)
        {
            for (unsigned int j=0; j<ant_configs.size(); ++j)
            {
                evaluation_ptr eval(new evaluation);
                pop[i]->evals.push_back(eval);
                unsigned int read = read_nou(str(nec_output % i % j), eval);
                if (read != (num_polar() * step_freq))
                    throw eap::InvalidStateException("Problem with output in " + str(nec_output % i % j));
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
    if(ant_configs.size() == placements.size())
    {
        pop.push_back(create_individual(str(nec_input % pop.size())+"a%02d.nec", placements));
        return;
    }

    for (unsigned int j=0; j<ant_configs[i]->positions.size(); j++)
    {
        if(!overlap(placements, ant_configs[i]->positions[j]))
        {
            placements.push_back(ant_configs[i]->positions[j]);
            recur_placements(placements, i+1);
            placements.pop_back();
        }
    }
}

exhaust::~exhaust(void)
{
}

