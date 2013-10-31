// SaveToLua.cpp : implementation file
//

#include "stdafx.h"
#include "APOT6.h"
#include "SaveToLua.h"
#include "PopulationBasedAlgos.h"
#include "SA.h"
#include "HC.h"
#include<float.h>
#include<fstream>
#include<iostream>


// SaveToLua

IMPLEMENT_DYNAMIC(SaveToLua, CWnd)
BEGIN_MESSAGE_MAP(SaveToLua, CWnd)
END_MESSAGE_MAP()


inline const char * const BoolToString(bool b)
{
  return b ? "true" : "false";
}


SaveToLua::SaveToLua()
{
    lua = "";
}

SaveToLua::~SaveToLua()
{
}

void SaveToLua::addPlatform(string platform)
{
    lua+="loadPlatform(\"" + platform + "\")\n";
}


void SaveToLua::addAntenna(string antenna_name)
{
    lua+="addAntenna(\""+antenna_name+"\")\n";
}

void SaveToLua::addPoint(double x, double y, double z)
{

    sprintf(str,"addPoint(%10f, %10f, %10f)\n", x, y, z);
    lua+=str;
}

void SaveToLua::setLocalRun(boolean state)
{
    sprintf(str,"setLocalRun(%s)\n", BoolToString(state));
    lua+=str;
}


void SaveToLua::setAlgorithm(unsigned int type)
{
    sprintf(str,"setAlgorithm(%d)\n", type);
    lua+=str;
}

void SaveToLua::setGAInfo(EAParams params)
{
    lua+="a = getGAInfo()\n";
    sprintf(str, "a[\"popSize\"]=%d\n", params.popSize);
    lua+=str;
    sprintf(str, "a[\"generations\"]=%d\n", params.generations);
    lua+=str;
    sprintf(str, "a[\"elitism\"]=%d\n", params.elitism);
    lua+=str;
    sprintf(str, "a[\"tournamentSize\"]=%d\n", params.tournamentSize);
    lua+=str;
    sprintf(str, "a[\"algorType\"]=%d\n", params.algorType);
    lua+=str;
    sprintf(str, "a[\"layers\"]=%d\n", params.layers);
    lua+=str;
    sprintf(str, "a[\"layerSize\"]=%d\n", params.layerSize);
    lua+=str;
    sprintf(str, "a[\"ageGap\"]=%d\n", params.ageGap);
    lua+=str;
    sprintf(str, "a[\"mutation\"]=%f\n", params.mutation);
    lua+=str;
    sprintf(str, "a[\"recombination\"]=%f\n", params.recombination);
    lua+=str;
    sprintf(str, "a[\"expWeight\"]=%f\n", params.expWeight);
    lua+=str;
	sprintf(str, "a[\"mutationType\"]=%d\n", params.mutationType);
    lua+=str;
    sprintf(str, "a[\"seed\"]=%d\n", params.seed);
    lua+=str;
	/*tbll - autoSeed boolean*/
    //lua+=sprintf(str, "a[\"popSize\"]=%d\n", params.popSize);
    lua+="setGAInfo(a)\n";
}

void SaveToLua::setSAInfo(SAParams params)
{
    lua+="a = getSAInfo()\n";
    sprintf(str, "a[\"iterations\"]=%d\n", params.iterations);
    lua+=str;
    sprintf(str, "a[\"mutationProbability\"]=%f\n", params.mutationProbability);
    lua+=str;
    sprintf(str, "a[\"seed\"]=%d\n", params.seed);
    lua+=str;
    sprintf(str, "a[\"initialTemp\"]=%f\n", params.initialTemp);
    lua+=str;
    sprintf(str, "a[\"tempFactor\"]=%f\n", params.tempFactor);
    lua+=str;
    sprintf(str, "a[\"iterationsPerTempChng\"]=%d\n", params.iterationsPerTempChng);
    lua+=str;
    sprintf(str, "a[\"outputFreq\"]=%d\n", params.outputFreq);
    lua+=str;
    sprintf(str, "a[\"convergenceFactor\"]=%f\n", params.convergenceFactor);
    lua+=str;
    sprintf(str, "a[\"genCondition\"]=%d\n", params.genCondition);
    lua+=str;
    sprintf(str, "a[\"fitnessCondition\"]=%d\n", params.fitnessCondition);
    lua+=str;
	sprintf(str, "a[\"mutationType\"]=%d\n", params.mutationType);
    lua+=str;
    sprintf(str, "a[\"expWeight\"]=%f\n", params.expWeight);
    lua+=str;
	lua+="setSAInfo(a)\n";
}

void SaveToLua::setHCInfo(HCParams params)
{
    lua+="a = getHCInfo()\n";
    sprintf(str, "a[\"iterations\"]=%d\n", params.iterations);
    lua+=str;
    sprintf(str, "a[\"mutationProbability\"]=%f\n", params.mutationProbability);
    lua+=str;
    sprintf(str, "a[\"seed\"]=%d\n", params.seed);
    lua+=str;
    sprintf(str, "a[\"outputFreq\"]=%d\n", params.outputFreq);
    lua+=str;
    sprintf(str, "a[\"convergenceFactor\"]=%f\n", params.convergenceFactor);
    lua+=str;
    sprintf(str, "a[\"genCondition\"]=%d\n", params.genCondition);
    lua+=str;
    sprintf(str, "a[\"fitnessCondition\"]=%d\n", params.fitnessCondition);
	lua+=str;
	sprintf(str, "a[\"mutationType\"]=%d\n", params.mutationType);
    lua+=str;
    sprintf(str, "a[\"expWeight\"]=%f\n", params.expWeight);
    lua+=str;
	sprintf(str, "a[\"seed\"]=%d\n", params.seed);
    lua+=str;
    lua+="setHCInfo(a)\n";
}


void SaveToLua::setESInfo(ESParams params)
{
    lua+="a = getESInfo()\n";
    sprintf(str, "a[\"generations\"]=%d\n", params.generations);
    lua+=str;
    sprintf(str, "a[\"mutation_probability\"]=%f\n", params.mutation_probability);
    lua+=str;
    sprintf(str, "a[\"seed\"]=%d\n", params.seed);
    lua+=str;
	sprintf(str,"setAutoSeed(%s)\n", BoolToString(params.autoSeed));
    lua+=str;
    sprintf(str, "a[\"mu\"]=%d\n", params.mu);
    lua+=str;
    sprintf(str, "a[\"lambda\"]=%d\n", params.lambda);
    lua+=str;
	sprintf(str, "a[\"mutationType\"]=%d\n", params.mutationType);
    lua+=str;
    sprintf(str, "a[\"expWeight\"]=%f\n", params.expWeight);
    lua+=str;
	lua+="setESInfo(a)\n";
}

void SaveToLua::writeToLua(const char* filename)
{
    char buffer[100];
    ofstream fp (filename);
    if (fp.is_open())
        fp << lua;
    fp.close();
}


