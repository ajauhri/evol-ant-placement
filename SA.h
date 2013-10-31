#pragma once

#include <vector>
#include "Geometry.h"
#include "NECSimulator.h"
#include "APOT6.h"

typedef struct SAParams
{
    CWnd*			hWindow;
    unsigned int iterations; //u
	bool autoSeed;
    float mutationProbability; //u
    unsigned int seed;
    float initialTemp; //u
    float tempFactor;  //u
    unsigned int iterationsPerTempChng; //u
    unsigned int outputFreq;  //u
    float convergenceFactor; //u
    float expWeight;
	unsigned mutationType;

    /* STOP CONDITIONS for SA */
    unsigned int genCondition;
    unsigned int fitnessCondition; //nu

    /* Temporary */
    CString username;
    CString password;
} SAParams;


class SA
{
public:
    SA(void);
    ~SA(void);
    static bool flip(float probability);
};

extern SAParams				SAInfo;
//extern bool SAparams_valid(void);
//extern vector<individualSA*>	outputs;