#pragma once

#include <vector>
#include "Geometry.h"
#include "NECSimulator.h"
#include "APOT6.h"

typedef struct ESParams
{
    CWnd* hWindow;
    unsigned int mu ;
    unsigned int generations;
    bool            autoSeed;
    unsigned int    seed;         // random number seed
    unsigned int lambda;
    float mutation_probability;
    float	expWeight;
	unsigned mutationType;
} ESParams;

extern ESParams				ESInfo;