// antennaGA.h
//
// This file contains the antenna specific portions of the GA, such as how to evaluate and encode different
// representations.

#pragma once

#include <vector>
#include "Geometry.h"
#include "NECSimulator.h"
#include "APOT6.h"
#include "PopulationBasedAlgos.h"

/*
  Defining FITNESS_CACHE allows one to use a cache so that individuals
  aren't constantly re-evaluated every generation.  (For the runs that
  I've executed a 7 hour run can be done in just 2 hours.)  However,
  it bypasses the RunBatch() function in NECSimulator, which HALLE
  needs.  Instead it uses RunIndividual().  The current solution is to
  turn off fitness caching and go back through the RunBatch() pathway.

  The better solution in the long run, suggested by Bill Kraus, is to
  allow finer grained control of what individuals are to be evaluated.
  Perhaps, change RunIndividual() to AddIndividualToBatch() that would
  collect the individual, then RunBatch() would assemble that set for
  execution batch style. -SEC
*/
//#define FITNESS_CACHE 1

#ifdef FITNESS_CACHE
#include "FitnessCache.h"
#endif

#define WORST_FITNESS	1e10f

int statusf(char *format, ...);

using namespace std;

#define MUTATION_STD 0.01

// algorType values
#define STDGA   0
#define ALPS    1
#define EXHAUST 2

// representation values
#define INDEX_REP   0
#define SURFACE_REP 1


// mutation types
#define SIMPLE_MUTATION 0
#define GLOBAL_NEIGHBOURHOOD_MUTATION 1
#define LOCAL_NEIGHBOURHOOD_MUTATION 2

typedef struct EAParams
{
    CWnd*			hWindow;
    unsigned int	popSize;	// Might want to get rid of popSize in favor of using layers and layerSize,
    unsigned int	generations;  // which in the degenerate case can emulate a standard GA with population
    unsigned int    elitism;      // size.
    unsigned int    tournamentSize;
//float         tournamentProb;
    unsigned        algorType;    // algorithm type
	unsigned		mutationType;
// begin alps
//bool            alps;
    unsigned int    layers;
    unsigned int    layerSize;
    unsigned int    ageGap;
// end alps
    float			mutation;
    float			recombination;
    float			expWeight;
    bool            autoSeed;
    unsigned int    seed;         // random number seed
    unsigned        representation;
    CString			username;
    CString			password;
} EAParams;

class ValidPoint
{
public:
	float distance;
	int id;
};

/*abaj*/
class antenna_configuration
{
private:
	int coupling_point;
public:
    antenna_configuration(void);
    antenna_configuration(const antenna_configuration&);
    ~antenna_configuration(void);

    void initialize(vector<antenna*>);
    antenna_configuration* clone(bool deep) const;
    void print(vector<antenna*>, bool all = true) const;
    void setup(unsigned int id, signed int generation = -1);
    void evaluate(unsigned int id, vector<antenna*>, float weight, signed int generation = -1);
	void write(const char* filename, unsigned int which) const;
	void write_coupling(const char* filename, unsigned int id) const;
    void simulate(NECSimulator* sim, unsigned int id, signed int generation = -1);
    vector<antenna_configuration*> breed(const antenna_configuration* mate);
    antenna_configuration* simple_mutation(float mutation, vector<antenna*>);
	antenna_configuration* global_neighbourhood_mutation(float mutation, vector<antenna*>);
	antenna_configuration* local_neighbourhood_mutation(float mutation, vector<antenna*>);
	void get_global_neighbourhood(point* antenna_point, vector<point*> *valid_points, vector<int>*);
	void get_local_neighbourhood(point* antenna_point, vector<point*> *valid_points, vector<int>*);

    bool                antennas_overlap();
    bool				check_overlap(point*);
    void                surface_to_coords(vector<antenna*>);
    void                fix_antennas_overlap(vector<antenna*>);
    void		        build_antenna(vector<antenna*>);
    void		        free_display_lists();
    point*              map_point(unsigned antenna_index, vector<antenna*> antennas) const;
    unsigned	        to_number(vector<antenna*>);
    void    	        from_number(unsigned, vector<antenna*>);
    unsigned    	    max_number(vector<antenna*>);

    vector <antenna*> ants;
    vector <evaluation*> results;
    vector<float> one_antenna_on_fitness;
    float fitness;
    int age;
    int gen_found;

};


//AJ
#if 0
class antenna_individual : public individual
{
public:
    antenna_individual(void);
    virtual ~antenna_individual(void);

    virtual void        initialize(vector<antenna*>);
    virtual individual* clone(bool deep, vector<antenna*>) const;
    virtual void        print(vector<antenna*>, bool all = true) const;
    virtual void		setup(unsigned int id, vector<antenna*>, signed int generation = -1);
    virtual void		evaluate(unsigned int id, vector<antenna*>, float weight, signed int generation = -1);
    virtual void		write(const char* filename, unsigned int which, vector<antenna*>) const;
    virtual void        simulate(NECSimulator* sim, unsigned id, vector<antenna*>, signed generation = -1);
    virtual individual* breed(const individual* mate, vector<antenna*> antenna);
    virtual individual* mutate(float mutation, vector<antenna*>);

    bool                antennas_overlap(vector<antenna*>);
    void                surface_to_coords(vector<antenna*>);
    void                fix_antennas_overlap(vector<antenna*>);
    void		        build_antenna(vector<antenna*>);
    void		        free_display_lists();
    point*              map_point(unsigned antenna_index, vector<antenna*> antennas) const;
    unsigned	        to_number(vector<antenna*>);
    void    	        from_number(unsigned, vector<antenna*>);
    unsigned    	    max_number(vector<antenna*>);

    unsigned int*		coords;           // INDEX_REP Representation: index to the coordinates.
    struct surface
    {
        float x;
        float y;
    }* surface; // SURFACE_REP Representation: [0,1] surface
    vector<evaluation*> results;
    antenna*			ant;
};
#endif




//AJ
#if 0
// The sim_population is a standard GA equipped to evaluate using the antenna simulation.
class sim_population : public population
{
public:
    virtual void        initialize(int size);
    virtual individual* new_individual(void);
    virtual void		evaluate(unsigned generation);
#ifdef FITNESS_CACHE
    FitnessCache cache;
#endif
};
#endif

//AJ
#if 0
// The sim_population is a ALPS GA equipped to evaluate using the antenna simulation.
class sim_alps_population : public alps_population
{
    virtual void        initialize(int size);
    virtual individual* new_individual(void);
    virtual void		evaluate(unsigned generation);
#ifdef FITNESS_CACHE
    FitnessCache cache;
#endif
};

#define MAX_OUT 1

class exhaustive_search : public sim_population
{
    virtual ~exhaustive_search();
    virtual void        initialize(int size);
    virtual void		evaluate(unsigned generation);
    virtual void		select();

    antenna_individual* ind;
    FILE *out[MAX_OUT];
};
#endif

#define LOCAL_NEC	NEC2Simulator

#define GLOBAL_NEC	HalleNECSimulator

extern NECSimulator*		Simulator;
extern EAParams				EAInfo;
/*-- To store the best confirations in case of a GA.
  -- Stores the output configuration in case of SA, HC, or RS */
extern vector<antenna_configuration*>	outputs;

extern bool GAparams_valid(void);

extern bool SAparams_valid(void);

extern bool HCparams_valid(void);

extern bool ESparams_valid(void);

extern BOOL SearchThreadRunning;
extern BOOL SearchAbortThread;

extern BOOL RunLocal;

extern int rand_integer(int min_value, int max_value);
extern double rand_double();

extern int distance_sort(ValidPoint*, ValidPoint*);
