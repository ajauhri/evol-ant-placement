#include "stdAfx.h"
#include "PopulationBasedAlgos.h"
#include "AntennaConfiguration.h"
#include "StatusMessages.h"
#include<algorithm>

using namespace std;

void alps_population::initialize(unsigned int size)
{
    mutation = EAInfo.mutation;
    popsize = size;
    elitism = EAInfo.elitism;
    recombination = EAInfo.recombination;
    tournamentSize = EAInfo.tournamentSize;
    /*tbll*/
    //alps_pop.push_back(vector<antenna_configuration*>());
    layer_count = 1;
    fibonacci_list[0] = 1;
    fibonacci_list[1] = 1;
    age_gap_factor = 20;
}


void alps_population::evaluate(unsigned int generation)
{
    // make local GEN directory
    char buffer[100];
    count = 0;
    sprintf(buffer, "C:\\cygwin\\bin\\mkdir ./GEN%04d", generation);
    execute(buffer);

    for (unsigned int l = 0; l < layer_count; l++)
    {
        for (unsigned int i = 0; i < alps_pop.at(l).size(); i++, count++)
        {
            alps_pop.at(l).at(i)->setup(i, generation);
            if (SearchAbortThread) return;
        }
    }

    statusf(START_SIM);
    Simulator->RunBatch(generation, 0, count);

    if (SearchAbortThread) return;

    Simulator->Retrieve(generation);
    count = 0;
    for (unsigned int l = 0; l < layer_count; l++)
    {
        for (unsigned int i = 0; i < alps_pop.at(l).size(); i++, count++)
        {
            statusf(EVAL_IND, count, generation);
            alps_pop.at(l).at(i)->evaluate(count, antennasEA, EAInfo.expWeight, generation);
            if (SearchAbortThread) return;
        }
    }

}

void alps_population::select()
{
    if (generation == 0)
        initialize_layer_zero_population();
    else if (generation % (age_gap_factor * fibonacci_list.at(layer_count)) == 0)
    {
        initialize_layer_zero_population();
        layer_count += 1;
        update_fibonacci_list();
        next_generation();
    }
    else
    {
        upgrade_antenna_configurations();
        next_generation();

    }
    increment_population_age();

}

void alps_population::upgrade_antenna_configurations()
{
    for (int layer_number = layer_count; layer_number >=0; layer_number --)
    {
        vector<antenna_configuration*> layer_population = alps_pop.at(layer_number);

        int fibonacci_factor = fibonacci_list.at(layer_number);
        int layer_age_limit = age_gap_factor * fibonacci_factor;

        for (int i = 0; i < layer_population.size(); i++)
        {
            if (layer_population.at(i)->age > layer_age_limit)
            {
                put_configuration_in_succeeding_layer(layer_population[i], layer_number + 1);
                layer_population.erase(layer_population.begin()+i, layer_population.begin() + (i + 1));
                i--;
            }
        }
    }
}


void alps_population::next_generation()
{
    for (int layer_number = layer_count; layer_number >= 0; layer_number--)
    {
        for (int ii = elitism ; ii < alps_pop.at(layer_number).size(); ii++)
        {
            antenna_configuration *parent2, child;

            if (rand_double() < 0.5)
            {
                parent2 = alps_pop[layer_number][rand_integer(0, elitism)];
            }
            else
            {
                try
                {
                    parent2 = alps_pop[layer_number - 1][rand_integer(0, elitism)]->clone(false);
                }
                catch (out_of_range* exce)
                {
                    parent2 = alps_pop[layer_number][rand_integer(0, elitism)]->clone(false);
                }
            }
			/*tbll*/
            //alps_pop[layer_number][ii] = alps_pop[layer_number][ii]->breed(parent2);
            alps_pop[layer_number][ii] = alps_pop[layer_number][ii]->simple_mutation(mutation, antennasEA);
        }
    }
}

void alps_population::update_fibonacci_list()
{
    fibonacci_list.push_back(fibonacci_list[fibonacci_list.size() - 1] + fibonacci_list[fibonacci_list.size() - 2]);
}



void alps_population::increment_population_age()
{
    for (int layer_number = 0; layer_number < layer_count; layer_number++)
    {
        vector<antenna_configuration*> layer_population = alps_pop[layer_number];
        for (int i = 0; i < layer_population.size(); i++)
            layer_population[i]->age += 1;
    }
}


void alps_population::put_configuration_in_succeeding_layer(antenna_configuration *conf, unsigned int layer_number)
{
    vector<antenna_configuration*> layer_population = alps_pop[layer_number];
    int last_configuration = layer_population.size() - 1;
    if (last_configuration < 0)
        layer_population.push_back(conf);
    else if (layer_population[last_configuration]->fitness < conf->fitness)
    {
        layer_population.at(last_configuration) = conf;
        sort_population_by_fitness(layer_number);
    }
}


void alps_population::sort_population_by_fitness(unsigned int layer_number)
{
    std::sort(alps_pop[layer_number].begin(), alps_pop[layer_number].end(), fitness_sort);
}

/* At regular intervalis (fibonacci sequence in this case), the search needs to be restarted in first layer */

void alps_population::initialize_layer_zero_population()
{
    vector<antenna_configuration*> layer_population;
    vector< vector<antenna_configuration*> >::iterator it;

    for (unsigned int ii = 0; ii < popsize; ii++)
    {
        antenna_configuration *configuration = new antenna_configuration;
        configuration->initialize(antennasEA);
        layer_population.push_back(configuration);
    }
    it = alps_pop.begin();
    alps_pop.insert(it,layer_population);
    sort_population_by_fitness(layer_count - 1);
}
