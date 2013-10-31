#include "stdafx.h"
#include "SA.h"

SAParams SAInfo;

SA::SA(void)
{
}


SA::~SA(void)
{
}

//This function flips a biased coin with the probability 'probability'
//of having a 1 returned. (flip(0.75) returns a 1 3/4 of the time)

bool SA::flip(float probability)
{
    float random = (float) ((double)rand() / ((double)RAND_MAX +1) * 10000) / 10000;

    if (random < probability)
        return TRUE;
    else
        return FALSE;
}

