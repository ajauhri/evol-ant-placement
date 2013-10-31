// PopulationBasedAlgos.h
//
// PopulationBasedAlgos.h defines the classes individual, population, and alps_population.  It also includes
// scalar_individual, which is for testing purposes.  It defines enough to run an evolutionary
// algorithm and has no references to what the particularly evaluation of an individual is (e.g.,
// there is no antenna specific code in these classes).  The particulars of how individuals are
// encoded and what they represent are expected to be provided by subclasses of individual.

#pragma once
#include <iostream>
#include "Geometry.h"
#include "NECSimulator.h"

#define WORST_FITNESS	1e10f

float randf(float start, float end);
float gauss();
float gaussf(float mean, float std);
float bound(float value, float min, float max);
//float rand_integer(int min_value, int max_value);

using namespace std;

//AJ
#if 0
class individual
{
public:
    individual(void);
    virtual ~individual(void);

    virtual void        initialize(vector<antenna*>);
    virtual individual* clone(bool deep, vector<antenna*>) const;
    virtual void        print( vector<antenna*>, bool all = true) const;
    virtual void		setup(unsigned int id, vector<antenna*>, signed int generation = -1);
    virtual void		evaluate(unsigned int id, vector<antenna*>, float weight, signed int generation = -1);
    virtual void		write(const char* filename, unsigned int which, vector<antenna*>) const;
    virtual void		simulate(NECSimulator* sim, unsigned id, vector<antenna*>, signed generation = -1);

    virtual individual* breed(const individual* mate, vector<antenna*>);
    virtual individual* mutate(float mutation, vector<antenna*>);

    float				fitness;
    unsigned            age;
};
#endif

//AJ
#if 0
// A simple dumb individual whose genes consist of one scalar number.  Useful for exercising
// the GAs to ensure they are functioning correctly.
class scalar_individual : public individual
{
public:
    scalar_individual();
    virtual individual* clone(bool deep, vector<antenna*>) const;
    virtual void		evaluate(unsigned int id, vector<antenna*>, float weight, signed int generation);
    virtual individual* breed(const individual* mate, vector<antenna*>);
    virtual individual* mutate(float mutation, vector<antenna*>);
    float number;
};


// Standard EA Population
class population
{
public:
    population(void)
    {
        individuals = 0;
        popsize = 0;
        elitism = 0;
        tournamentSize = 5;
        recombination = 0.5f;
        mutation = 0.5f;
    };
    virtual ~population(void);

    virtual void        initialize(int size);
    virtual void        select(void);
    virtual void        rank(void);
    virtual void        evaluate(unsigned generation);
    virtual individual* get_best(void);
    virtual individual* new_individual(void);
    virtual unsigned    pop_size(void);

    individual**  individuals;
    unsigned elitism;
    unsigned tournamentSize;
    float recombination;
    float mutation;

    friend ostream& operator<<(ostream& out, population& pop);
protected:
    individual* tour(void);
private:
    int           popsize;
};
#endif

class antenna_configuration;

class population
{
public:
    population(void)
    {
        popsize = 0;
        elitism = 0;
        tournamentSize = 5;
        recombination = 0.5f;
        mutation = 0.5f;
    };
    virtual ~population(void);

    virtual void        initialize(unsigned int size);
    virtual void        select(void);
    virtual void        evaluate(unsigned generation);
    virtual void        sort_by_fitness(void);
    virtual antenna_configuration* get_best(void);
    virtual unsigned    pop_size(void);
    virtual void setPopSize(unsigned int);
    virtual void setPopForES(vector<antenna_configuration*>);

    vector<antenna_configuration*> pop;
    vector<antenna_configuration*> buffer;
    unsigned elitism;
    unsigned tournamentSize;
    float recombination;
    float mutation;
    /*tbll*/
//friend ostream& operator<<(ostream& out, population& pop);
protected:
    antenna_configuration* tour(void);
private:
    int           popsize;
};


// The sim_population is a standard GA equipped to evaluate using the antenna simulation.
class sim_population : public population
{
public:
    virtual void        initialize(unsigned int size);
    //virtual individual* new_individual(void);
    virtual void		evaluate(unsigned generation);
#ifdef FITNESS_CACHE
    FitnessCache cache;
#endif
};

//AJ

// ALPS EA Population
class alps_population : public population
{
public:
    alps_population(void)
    {
        ;
    };
    antenna_configuration*         tour_alps(unsigned layer);
    virtual void        select(void);
    virtual void        initialize(unsigned int size);
    virtual void        evaluate(unsigned generation);
    friend ostream& operator<<(ostream& out, alps_population& pop);
//protected:
    void         promote_based_on_age();
    unsigned     max_occupied_layer();
    unsigned     maxLayer();
    unsigned     layer_start(unsigned layer);
    unsigned     layer_end(unsigned layer);
    unsigned     max_age(unsigned layer);
    int          worst_fitness(unsigned layer);
    void         seed_new_layer0();
    void         check_ages();
    void         open_new_layer();

    unsigned layers;
    unsigned layerSize;
    unsigned generation;
    unsigned ageGap;
    unsigned curMaxLayer;

    unsigned int popsize;
    unsigned int layer_count;
    unsigned int age_gap_factor;
    unsigned int count;
    vector<int> fibonacci_list;
    vector< vector<antenna_configuration*> > alps_pop;
    void initialize_layer_zero_population();
    void sort_population_by_fitness(unsigned int);
    void update_fibonacci_list();
    void upgrade_antenna_configurations();
    void next_generation();
    void increment_population_age();
    void put_configuration_in_succeeding_layer(antenna_configuration* , unsigned int);
};

/*abaj*/
class evolutionary_strategy : public population
{

public:
    virtual void initialize(unsigned int size);
    virtual void mutate(unsigned int type);
    virtual void evaluate(unsigned int);
    virtual void survivor_selection();
    unsigned int mulambda_factor;
};

/*abaj*/
class exhaustive_search : public population
{
public:
    virtual void        initialize(unsigned int size);
    virtual void		evaluate(unsigned int generation);
    virtual void		makeConfigurations(antenna_configuration*, unsigned int);

};

extern int fitness_sort(antenna_configuration*, antenna_configuration*);