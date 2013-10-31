// SimPopulation.cpp
//
// This code is the same exact code for both sim_population and sim_alps_population, but because of the
// way the classes are structured it's easier to just generate sim_alps_population from this file rather
// than resorting to multiple inheritance or restructuring the class hierarchy.

#include "Geometry.h"
//AJ
#if 0
individual* sim_population::new_individual(void)
{
    individual* ind = new antenna_individual;
    ind->initialize(antennasGA);
    return ind;
}


void sim_population::evaluate(unsigned int generation)
{
    // make local GEN directory
    char buffer[100];
    sprintf(buffer, "C:\\cygwin\\bin\\mkdir ./GEN%04d", generation);
    execute(buffer);

    for (unsigned ii = 0; ii < pop_size(); ii++)
    {
        individuals[ii]->setup(ii, antennasGA, generation);
        if (gAbortThread) return;
    }

#ifdef FITNESS_CACHE
    // Here we have a cache and use the RunIndividual() pathway.
    for (unsigned i = 0; i < pop_size(); i++)
    {
        statusf("Simulating individual %d of generation %d", i, generation);
        antenna_individual* antind = (antenna_individual*) individuals[i];
        //gSimulator->RunIndividual(antind->infile, antind->outfile);

        if (! cache.HasFitness(antind))
        {
            antind->simulate(gSimulator, i, antennasGA,  generation);
        }

        if (gAbortThread) return;
    }
#else
    // Here we don't have a cache and use the RunBatch() pathway.
    statusf(START_SIM);
    gSimulator->RunBatch(generation, pop_size());
#endif

    if (gAbortThread) return;
    // Polling for results
    /* Commented by A.Jauhri */
    /*
    while (!gSimulator->Poll(generation, popsize)) {
    	Sleep(1000);
    	if (gAbortThread) return;
    }
    */
    gSimulator->Retrieve(generation);

#if NET_ENVIRON
    for (unsigned ii = 0; ii < pop_size(); ii++)
    {
#else
    for (ii = 0; ii < pop_size(); ii++)
    {
#endif
        statusf(EVAL_IND, ii, generation);
        antenna_individual* antind = (antenna_individual*) individuals[ii];
#ifdef FITNESS_CACHE
        if (cache.HasFitness(antind))
        {
            cache.GetFitness(antind);
        }
        else
        {
            individuals[ii]->evaluate(ii, antennasGA, GAInfo.expWeight, generation);
            cache.SetFitness(antind);
        }
#else
        individuals[ii]->evaluate(ii, antennasGA, GAInfo.expWeight, generation);
#endif
        if (gAbortThread) return;
    }
#ifdef FITNESS_CACHE
    cache.CheckOverflow();
#endif
}
#endif