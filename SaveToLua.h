#pragma once


// SaveToLua

#include<string>
#include "PopulationBasedAlgos.h"
#include "SA.h"
#include "HC.h"
#include "EvolutionaryStrategy.h"
#include "AntennaConfiguration.h"

using namespace std;
class SaveToLua : public CWnd
{
    DECLARE_DYNAMIC(SaveToLua)

public:
	SaveToLua();
    void addPlatform(string platform_name);
    void addAntenna(string antenna_name);
    void addPoint(double x, double y, double z);
    void setLocalRun(boolean);
    void setAlgorithm(unsigned int type);
    void setGAInfo(EAParams);
    void setSAInfo(SAParams);
    void setHCInfo(HCParams);
    void setESInfo(ESParams);
    void writeToLua(const char* filename);
    virtual ~SaveToLua();

private:
    char str[100];
    string lua;

protected:
    DECLARE_MESSAGE_MAP()
};


