#pragma once

#include <vector>
#include "Geometry.h"
#include "NECSimulator.h"
#include "APOT6.h"

typedef struct HCParams
{
    CWnd*			hWindow;
    unsigned int iterations; //u
    float mutationProbability; //u
    unsigned int seed;
	bool autoSeed;
    unsigned int outputFreq;  //u
    float convergenceFactor; //u
    float expWeight;
	unsigned mutationType;
    /* STOP CONDITIONS for HC */
    unsigned int genCondition;
    unsigned int fitnessCondition; //nu

    /* Temporary */
    CString username;
    CString password;
} HCParams;

class HC
{
public:
    HC(void);
    ~HC(void);
};

extern HCParams				HCInfo;