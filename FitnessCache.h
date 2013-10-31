#pragma once
#include <map>

using namespace std;
//AJ
#if 0
class antenna_individual;

struct FitnessRecord
{
    antenna_individual* ind;
    unsigned count; // How many times it's been accessed.
};

class FitnessCache
{
public:
    FitnessCache() {};
    ~FitnessCache() {};
    bool HasFitness(antenna_individual* ind);
    void GetFitness(antenna_individual* ind);
    unsigned GetCount(antenna_individual* ind);
    void SetFitness(antenna_individual* ind);
    void CheckOverflow();

    map <unsigned, FitnessRecord > cache;
};
#endif