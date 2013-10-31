#include "StdAfx.h"
#include "FitnessCache.h"
#include "AntennaConfiguration.h"
#include "Geometry.h"

//AJ
#if 0
bool
FitnessCache::HasFitness(antenna_individual* ind)
{
    return (cache.count(ind->to_number(antennasEA)) != 0);
}

void
FitnessCache::GetFitness(antenna_individual* ind)
{
    if (! HasFitness(ind))
        assert(FALSE);

    unsigned k = ind->to_number(antennasEA);
    antenna_individual* cind = cache[k].ind;
    cache[k].count++;
    ind->ant = cind->ant ? new antenna(*(cind->ant)) : NULL;
    for (unsigned i = 0; i < cind->results.size(); i++)
    {
        ind->results.push_back(new evaluation(*(cind->results[i])));
    }
    ind->fitness = cind->fitness;
}

void
FitnessCache::SetFitness(antenna_individual* ind)
{
    if (HasFitness(ind))
        assert(FALSE);
    unsigned k = ind->to_number(antennasEA);

    cache[k].ind = (antenna_individual*) ind->clone(TRUE, antennasEA);
    cache[k].count = 0;
}

void
FitnessCache::CheckOverflow()
{
    // We'll just clear it out now and then.  This'll cause some
    // reevaluations and that's okay.
    if (cache.size() >= 200)
    {
        map<unsigned, FitnessRecord>::iterator iter;
        for (iter = cache.begin(); iter != cache.end(); iter++)
        {
            if (iter->second.ind)
                delete iter->second.ind;
            //cout << (*iter).first << " is the first character in the word " << iter->second << endl;
        }
        cache.clear();
    }
}

#endif