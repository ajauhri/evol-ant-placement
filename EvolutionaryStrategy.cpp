#include "stdAfx.h"
#include "PopulationBasedAlgos.h"
#include "AntennaConfiguration.h"
#include "StatusMessages.h"
#include "EvolutionaryStrategy.h"
#include <algorithm>
#include <math.h>

using namespace std;

ESParams ESInfo;

void
evolutionary_strategy::initialize(unsigned int size)
{
    mulambda_factor = floor((long double) ESInfo.lambda/ESInfo.mu);
    mutation = ESInfo.mutation_probability;
    for (unsigned int i = 0; i < size; i++)
    {
        antenna_configuration* configuration = new antenna_configuration;
        configuration->initialize(antennasES);
        pop.push_back(configuration);
    }
}


/* @brief Performs mutation on population (of mu size) lambda times
 * @param type define the type of mutationt to be performed
 */
void
evolutionary_strategy::mutate(unsigned int type)
{
	if (type == SIMPLE_MUTATION)
	{
		//vector<antenna_configuration*> population;
		for	(unsigned int i = 0; i < ESInfo.mu; i++)
		{
			 int counter = 0;
			 while (counter <  mulambda_factor)
			{
				 pop.push_back(pop[i]->simple_mutation(mutation, antennasES));
				 counter ++;
			}	
			//free(pop[i]);
		}
		//pop = population;
	}

	else if (type == GLOBAL_NEIGHBOURHOOD_MUTATION)
	{
		for	(unsigned int i = 0; i < ESInfo.mu; i++)
		{
			 int counter = 0;
			 while (counter < mulambda_factor)
			{
				 pop.push_back(pop[i]->global_neighbourhood_mutation(mutation, antennasES));
				 counter ++;
			}	
		}
	}

	else if (type == LOCAL_NEIGHBOURHOOD_MUTATION)
	{
		for(unsigned int i = 0; i < ESInfo.mu; i++)
		{
			int counter = 0;
			while (counter < mulambda_factor)
			{
				pop.push_back(pop[i]->local_neighbourhood_mutation(mutation, antennasES));
				counter ++;
			}
		}
	}
}

void 
evolutionary_strategy::evaluate(unsigned int generation)
{
    // make local GEN directory
    char buffer[100];

    sprintf(buffer, "C:\\cygwin\\bin\\mkdir ./GEN%04d", generation);
    execute(buffer);


    for (unsigned ii = 0; ii < ESInfo.lambda + ESInfo.mu; ii++)
    {
        pop[ii]->setup(ii, generation);
        if (SearchAbortThread) return;
    }

    statusf(START_SIM);
    Simulator->RunBatch(generation, 0, ESInfo.mu + ESInfo.lambda);

    if (SearchAbortThread) return;

    Simulator->Retrieve(generation);

    for (unsigned ii = 0; ii < ESInfo.lambda + ESInfo.mu; ii++)
    {
        statusf(EVAL_IND, ii, generation);
        pop[ii]->evaluate(ii, antennasES, ESInfo.expWeight, generation);
        if (SearchAbortThread) return;
    }
}

void 
evolutionary_strategy::survivor_selection()
{
    pop.erase(pop.begin() + ESInfo.mu, pop.end());
    assert(pop.size() == ESInfo.mu);
}