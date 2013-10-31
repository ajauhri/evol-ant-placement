#include "stdafx.h"
#include "PopulationBasedAlgos.h"
#include <math.h>
#include <iostream>
#include <iomanip>
#include <assert.h>
#include "Geometry.h"
#include "AntennaConfiguration.h"
#include "StatusMessages.h"
#include <algorithm>

using namespace std;

/*abaj*/
void exhaustive_search::initialize(unsigned int size)
{
    /*tbll*/
    for (unsigned ii = 0; ii < outputs.size(); ii++) delete outputs[ii];
    outputs.clear();
    //makeConfiguration(antennasEA,0,1);
    antenna_configuration* configuration = new antenna_configuration();

    makeConfigurations(configuration, 0);

}


void exhaustive_search::makeConfigurations(antenna_configuration* configuration, unsigned int ii)
{
    if(configuration->ants.size() == antennasEA.size())
    {
        configuration->fitness = 0.0f;
        configuration->age = 0;
        pop.push_back(new antenna_configuration(*configuration));
        setPopSize(pop.size());
        return;
    }

    for (unsigned int jj=0; jj<antennasEA[ii]->points->size(); jj++)
    {
        if(!configuration->check_overlap(antennasEA[ii]->points->at(jj)))
        {
            antenna *ant = new antenna;
            strcpy(ant->label, antennasEA[ii]->label);
            for (unsigned int kk = 0; kk < antennasEA[ii]->wires.size(); kk++)
            {
                wire	wr(*(antennas[ii]->wires[kk]));
                ant->wires.push_back(new wire(wr));
            }

            ant->params = (analysis_params) antennasEA[ii]->params;
            point pt(*(antennasEA[ii]->points->at(jj)));
            ant->points->push_back(new point(pt));
            ant->color = antennas[ii]->color;
            configuration->ants.push_back(ant);
            makeConfigurations(configuration,ii+1);
            configuration->ants.pop_back();
        }
    }
}



/*abaj*/
#if 0
void exhaustive_search::makeConfigurations(vector<antenna*> antennas, unsigned int count, unsigned int combinations)
{
    if (count+1 != antennas.size())
        makeConfigurations(antennas, count+1, antennas[count]->points->size()*combinations);
    else
    {
        setPopSize(combinations*antennas[count]->points->size());
        for (int ii = 0; ii < pop_size(); ii++)
        {
            antenna_configuration* configuration = new antenna_configuration;
            pop.push_back(configuration);
        }
    }
    int curr_point_space = antennas[count]->points->size();
    int counter = 0;

    while(counter < pop_size())
    {
        for (int ii = 0; ii < curr_point_space; ii++)
        {
            for (int jj = 0; jj < combinations; jj++)
            {
                antenna* ant = new antenna;
                strcpy(ant->label, antennas[count]->label);
                for (unsigned int kk = 0; kk < antennas[count]->wires.size(); kk++)
                {
                    wire	wr(*(antennas[count]->wires[kk]));
                    ant->wires.push_back(new wire(wr));
                }

                ant->params = (analysis_params) antennas[count]->params;
                point pt(*(antennas[count]->points->at(ii)));
                ant->points->push_back(new point(pt));
                ant->color = antennas[count]->color;
                pop[counter++]->ants.push_back(ant);
            }
        }
    }
    //setPopForES(pop);
}
#endif
void 
exhaustive_search::evaluate(unsigned int generation)
{
    char buffer[100];
    sprintf(buffer, "C:\\cygwin\\bin\\mkdir ./GEN%04d", generation);
    execute(buffer);
	for (unsigned ii = 0; ii < pop_size(); ii++)
    {
        pop[ii]->setup(ii, generation);	
        if (SearchAbortThread) return;
    }
	
    statusf(START_SIM);
    Simulator->RunBatch(generation, 0, pop_size());

    if (SearchAbortThread) return;

    Simulator->Retrieve(generation);

#if NET_ENVIRON
	
    for (unsigned ii = 0; ii < pop_size(); ii++)
    {
#else
    for (ii = 0; ii < pop_size(); ii++)
    {
#endif
        statusf(EVAL_IND, ii, generation);
#ifdef FITNESS_CACHE
        if (cache.HasFitness(antind))
        {
            cache.GetFitness(antind);
        }
        else
        {
            individuals[ii]->evaluate(ii, antennasEA, EAInfo.expWeight, generation);
            cache.SetFitness(antind);
        }
#else
        pop[ii]->evaluate(ii, antennasEA, EAInfo.expWeight, generation);
#endif
        if (SearchAbortThread) return;
    }
}