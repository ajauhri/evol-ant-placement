//#ifndef UNIT_TEST
#include "stdafx.h"
//#endif
#include "PopulationBasedAlgos.h"
#include <math.h>
#include <iostream>
#include <iomanip>
#include <assert.h>
#include "Geometry.h"
#include "AntennaConfiguration.h"
#include "StatusMessages.h"
#include <algorithm>

using namespace std;

#define NET_ENVIRON 1

//AJ
#if 0
int compare_descend_fitness(const void* a, const void* b);
#endif



float randf(float start, float end)
{
    return (float) (((rand() % 10000) * (end - start))/10000.0 + start);
}


// Returns a "standard normal" (gaussian) distributed number.
float gauss()
{
    // from http://www.taygeta.com/random/gaussian.html
    // Algorithm by Dr. Everett (Skip) Carter, Jr.

    float x1, x2, w, y1, y2;

    do
    {
        x1 = 2.0 * randf(0.0, 1.0) - 1.0;
        x2 = 2.0 * randf(0.0, 1.0) - 1.0;
        w = x1 * x1 + x2 * x2;
    }
    while ( w >= 1.0 );

    w = sqrt( (-2.0 * log( w ) ) / w );
    y1 = x1 * w;
    // Actually has two random numbers, but we're throwing one away.
    //y2 = x2 * w;
    return y1;
}

// Returns a normally distributed random number with the given mean and standard deviation.
float gaussf(float mean, float std)
{
    return gauss() * std + mean;
}

// Clamp a value around its bounding min and max values.
float bound(float value, float min, float max)
{
    if (value < min)
        return min;
    else if (value > max)
        return max;
    else
        return value;
}

//AJ
#if 0
individual::individual(void)
{
    initialize(antennasEA);
}

individual::~individual(void)
{
}

void individual::initialize(vector<antenna*> antennas)
{
    age = 0;
    fitness = WORST_FITNESS;
}

void individual::setup(unsigned int id, vector<antenna*> antennas, signed int generation)
{
}

void individual::evaluate(unsigned int id, vector<antenna*> antennas, float weight, signed int generation)
{
}

void individual::write(const char* filename, unsigned int which, vector<antenna*> antennas) const
{
}

void individual::simulate(NECSimulator* sim, unsigned id, vector<antenna*> antennas, signed generation)
{
}

void individual::print(vector<antenna*> antennas, bool all) const
{
    cout << "F = " << fitness << "\n";
}

individual* individual::clone(bool deep, vector<antenna*> antennas) const
{
    individual* clone = new individual;
    clone->initialize(antennas);
    clone->fitness = fitness;
    clone->age = age;
    return clone;
}


individual* individual::breed(const individual* mate, vector<antenna*> antennas)
{
    individual* child = this->clone(false, antennas);
    child->fitness = WORST_FITNESS;
    // The breed used to do a mutation.  We're going to separate these two operations.
    child->age = max(this->age, mate->age) + 1;
    return child;
}


individual* individual::mutate(float mutation, vector<antenna*> antennas)
{
    individual* child = this->clone(false, antennas);
    /*	if ((rand() % 10000) < (mutation * 10000))
    		child->fitness = (float) rand();*/
    child->age++;
    child->fitness = WORST_FITNESS;
    return child;
}

#endif

//AJ
#if 0
individual* scalar_individual::clone(bool deep, vector<antenna*> antennas) const
{
    scalar_individual* clone = new scalar_individual;
    clone->initialize(antennas);
    clone->fitness = fitness;
    clone->age = age;
    clone->number = number;
    return clone;
}


individual* scalar_individual::breed(const individual* mate, vector<antenna*> antennas)
{
    scalar_individual* matey = (scalar_individual*) mate;
    scalar_individual* child = (scalar_individual*) this->clone(false, antennas);
    child->number = (number + matey->number)/2;
    child->fitness = WORST_FITNESS;
    // The breed used to do a mutation.  We're going to separate these two operations.
    child->age = max(this->age, matey->age) + 1;

    return child;
}

scalar_individual::scalar_individual()
{
    number = randf(0, 100);
}
void scalar_individual::evaluate(unsigned int id, vector<antenna*> antennas, float weight, signed int generation)
{
    fitness = number;
}

individual* scalar_individual::mutate(float mutation, vector<antenna*> antennas)
{
    scalar_individual* child = (scalar_individual*) this->clone(false, antennas);
    if (randf(0, 1.0f) < mutation)
    {
        child->number = gaussf(number, 0.1);
    }
    child->age++;
    child->fitness = WORST_FITNESS;
    return child;
}

#endif


/*abaj*/
void sim_population::initialize(unsigned int size)
{
    // Initialize operational parameters for the Genetic Algorithm
    mutation = EAInfo.mutation;
    elitism = EAInfo.elitism;
    recombination = EAInfo.recombination;
    tournamentSize = EAInfo.tournamentSize;

    /*tbll*/
    for (unsigned ii = 0; ii < outputs.size(); ii++) delete outputs[ii];
    outputs.clear();


    population::initialize(size);
}

/*abaj*/
void sim_population::evaluate(unsigned int generation)
{
    // make local GEN directory
    char buffer[100];

    sprintf(buffer, "C:\\cygwin\\bin\\mkdir ./GEN%04d", generation);
    execute(buffer);

    /* To ensure elitism is not valid for first generation  */
    //unsigned int elitism_copy = generation?elitism:0;

    for (unsigned ii = 0; ii < pop_size(); ii++)
    {
        pop[ii]->setup(ii, generation);
        if (SearchAbortThread) return;
    }

    statusf(START_SIM);
    Simulator->RunBatch(generation, 0, pop_size());

    if (SearchAbortThread) return;

    Simulator->Retrieve(generation);

    /*abaj*/
    /* Copies *.nec & *.nou from the previous generation which are to be carried forward without any mutations*/
    //Simulator->CopyEliteConfigurations(generation, elitism_copy);

#if NET_ENVIRON
    for (unsigned ii = 0; ii < pop_size(); ii++)
    {
#else
    for (ii = 0; ii < pop_size(); ii++)
    {
#endif
        statusf(EVAL_IND, ii, generation);
#ifdef FITNESS_CACHE
        if (cache.HasFitness(antind))
        {
            cache.GetFitness(antind);
        }
        else
        {
            individuals[ii]->evaluate(ii, antennasEA, EAInfo.expWeight, generation);
            cache.SetFitness(antind);
        }
#else
		pop[ii]->evaluate(ii, antennasEA, EAInfo.expWeight, generation); 
#endif
        if (SearchAbortThread) return;
    }
}

/*abaj*/
population::~population(void)
{
    pop.clear();
}

/*abaj*/
void population::setPopSize(unsigned int population)
{
    popsize = population;
}

void population::setPopForES(vector<antenna_configuration*> population)
{
    pop = population;
}



/*abaj*/
void population::initialize(unsigned int size)
{
    //assert(pop == NULL);
    popsize = size;

    for (unsigned int ii = 0; ii < pop_size(); ii++)
    {
        antenna_configuration* configuration = new antenna_configuration;
        configuration->initialize(antennasEA);
        pop.push_back(configuration);
    }

    //buffer.resize(pop_size());
}

/*abaj*/


inline void population::sort_by_fitness(void)
{
    //qsort(&pop[0], pop.size(), sizeof(antenna_configuration*), fitness_sort);
    std::sort(pop.begin(),pop.end(), fitness_sort);
}

int fitness_sort(antenna_configuration* pa, antenna_configuration* pb)
{
    return pa->fitness < pb->fitness;
}

#if 0
/*abaj*/
int fitness_sort(const void* a, const void* b)
{
    // this funky cast is required by qsort for arrays of pointers
    antenna_configuration* pa = *(antenna_configuration**) a;
    antenna_configuration* pb = *(antenna_configuration**) b;

    if (pb->fitness < pa->fitness)
        return 1;
    else if (pa->fitness < pb->fitness)
        return -1;
    return 0;
}
#endif

/*abaj*/
antenna_configuration* population::get_best(void)
{
    antenna_configuration* best = pop.at(0);
    return best;
}

/*abaj*/
// Implement a tournament selection
void population::select(void)
{
	buffer.clear();
    // Handle elitism.
    for (unsigned i = 0; i < elitism; i++)
    {
        buffer.push_back(pop[i]->clone(true));
        buffer[i]->age++;
    }

    for (unsigned i = elitism; i < pop_size(); i+=2)
    {
		antenna_configuration *parent1 = tour();
		antenna_configuration *parent2 = tour();

        if (randf(0, 1.0) < recombination)
		{
			vector<antenna_configuration*> temp;
            temp = parent1->breed(parent2);
			buffer.push_back(temp[0]);
			buffer.push_back(temp[1]);
			if (EAInfo.mutationType == SIMPLE_MUTATION) 
			{
				buffer[i] = buffer[i]->simple_mutation(mutation, antennasEA);
				buffer[i+1] = buffer[i+1]->simple_mutation(mutation, antennasEA);
			}
			else if (EAInfo.mutationType == GLOBAL_NEIGHBOURHOOD_MUTATION)
			{
				buffer[i] = buffer[i]->global_neighbourhood_mutation(mutation, antennasEA);
				buffer[i+1] = buffer[i+1]->global_neighbourhood_mutation(mutation, antennasEA);	
			}	
			else if (EAInfo.mutationType == LOCAL_NEIGHBOURHOOD_MUTATION)
			{
				buffer[i] = buffer[i]->local_neighbourhood_mutation(mutation, antennasEA);
				buffer[i+1] = buffer[i+1]->local_neighbourhood_mutation(mutation, antennasEA);
			}
		}
		else 
		{	
			buffer.push_back(parent1->clone(false));
			buffer.push_back(parent2->clone(false));
		}
    }


    for (unsigned ii = 0; ii < pop_size(); ii++) delete pop[ii];
	pop.clear();
    pop = buffer;
}


/*abaj*/
// Returns an individual from a tournament selection.
antenna_configuration* population::tour(void)
{
    int          tourSize = tournamentSize;
    antenna_configuration*  best = NULL;
    for (int i = 0; i < tourSize; i++)
    {
        /*tbll*/
        unsigned int r_temp = rand_integer(0, pop_size());
        antenna_configuration *temp = pop[r_temp];
        if (! best || temp->fitness < best->fitness)
        {
            best = temp;
        }
    }
    return best;
}



unsigned population::pop_size()
{
    return popsize;
}


/*abaj*/
void population::evaluate(unsigned int generation)
{

}



//AJ
#if 0
population::~population(void)
{
    if (individuals != NULL)
    {
        for (unsigned ii = 0; ii < pop_size(); ii++) delete individuals[ii];
        delete[] individuals;
    }
}

individual* population::new_individual()
{
    individual* ind;
    //ind = new individual();
    ind = new scalar_individual();
    ind->initialize(antennasEA);
    return ind;
}


void population::initialize(int size)
{
    assert(individuals == NULL);

    popsize = size;
    individuals = new individual*[pop_size()];
#if NET_ENVIRON
    for (unsigned ii = 0; ii < pop_size(); ii++)
    {
#else
    for (ii = 0; ii < pop_size(); ii++)
    {
#endif
        individuals[ii] = new_individual();
    }
}

void population::rank(void)
{
    qsort(individuals, pop_size(), sizeof(individual*), compare_descend_fitness);
}

void population::evaluate(unsigned int generation)
{
    for (unsigned ii = 0; ii < pop_size(); ii++)
    {
        individuals[ii]->setup(ii, antennasEA, generation);
    }
#if NET_ENVIRON
    for (unsigned ii = 0; ii < pop_size(); ii++)
    {
#else
    for (ii = 0; ii < pop_size(); ii++)
    {
#endif
        //statusf("Evaluating individual %d of generation %d...", ii, generation);
        individuals[ii]->evaluate(ii, antennasEA, EAInfo.expWeight, generation);
    }
}


// Implement a tournament selection
void population::select(void)
{
    individual** temp = new individual*[pop_size()];
    memset(temp, 0, sizeof(individual*) * pop_size());
    rank();
    // Handle elitism.
    for (unsigned i = 0; i < elitism; i++)
    {
        temp[i] = individuals[i]->clone(true, antennasEA);
        temp[i]->age++;
    }
    for (unsigned ii = elitism; ii < pop_size(); ii++)
    {
        if (randf(0, 1.0) < recombination)
        {
            temp[ii] = tour()->breed(tour(), antennasEA);
        }
        else
        {
            temp[ii] = tour()->mutate(mutation, antennasEA);
        }
    }

#if NET_ENVIRON
    for (unsigned ii = 0; ii < pop_size(); ii++) delete individuals[ii];
#else
    for (ii = 0; ii < pop_size(); ii++) delete individuals[ii];
#endif
    delete[] individuals;
    individuals = temp;
}

individual* population::get_best(void)
{
    individual *best = NULL;
    for (unsigned i = 0; i < pop_size(); i++)
    {
        if (!best || individuals[i]->fitness <= best->fitness)
        {
            best = individuals[i];
        }
    }
    return best;
}

// Returns an individual from a tournament selection.
individual* population::tour(void)
{
    int          tourSize = tournamentSize;
    individual*  best = NULL;
    for (int i = 0; i < tourSize; i++)
    {
        individual* temp = individuals[rand() % pop_size()];
        if (! best || temp->fitness < best->fitness)
        {
            best = temp;
        }
    }
    return best;

    /*
       // ref: http://en.wikipedia.org/wiki/Tournament_selection

       individual** tour     = new individual*[tourSize];
       individual*  result   = NULL;

       // Grab a bunch of random individuals.
       for (int i = 0; i < tourSize; i++) {
    	   tour[i] = individuals[rand() % pop_size()];
       }
       qsort(tour, tourSize, sizeof(individual*), compare_descend_fitness);
       // Should this tournament probability be parameterized,
       // or is there a good value we feel comfortable with for it?
       float tourProb = 0.5f;
       for (int i = 0; i < tourSize; i++) {
    	   if ((rand() % 10000) < ((tourProb * powf(1 - tourProb, i)) * 10000)) {
               result = tour[i];
    		   break;
    	   }
       }
       if (! result)
    	   result = tour[tourSize - 1];
       delete[] tour;
       return result;
    */
}


#endif


//AJ
#if 0
void alps_population::rank(void)
{
    // Sort each layer WRT its fitness.  No jumps between layers here.
    for (unsigned n = 0; n <= max_occupied_layer(); n++)
    {
        qsort(individuals + layer_start(n), layerSize, sizeof(individual*), compare_descend_fitness);
    }
}


// Get individual with worst fitness for layer n.
int alps_population::worst_fitness(unsigned n)
{
    int worst = -1;
    for (unsigned i = layer_start(n); i <= layer_end(n); i++)
    {
        if (worst == -1 || individuals[worst]->fitness < individuals[i]->fitness)
            worst = i;
    }
    return worst;
}

void alps_population::initialize(int size)
{
    layers = 5;
    layerSize = 5;
    generation = 0;
    elitism = 2;
    tournamentSize = 3;
    recombination = 0.5f;
    mutation = 0.5f;
    ageGap = 10;
    curMaxLayer = 0;
    population::initialize(pop_size());
}

// Checks the ages of each individual WRT their layers and makes sure
// none are over the max age.
void alps_population::check_ages()
{
    for (unsigned n = 0; n <= max_occupied_layer(); n++)
    {
        for (unsigned i = layer_start(n); i <= layer_end(n); i++)
        {
            individual* ind = individuals[i];
            if (n == 0 && maxLayer() == 0)
            {
                // No constraints on age.
            }
            else if (n == 0)
            {
                assert(ind->age <= max_age(0));
            }
            else if (n == maxLayer())
            {
                //assert(ind->age >= max_age(n - 1));
            }
            else
            {
                assert(ind->age <= max_age(n));
                //assert(ind->age >= max_age(n - 1));
            }
        }
    }
}

void alps_population::seed_new_layer0()
{
    cout << "seed_new_layer0" << endl;
    for (unsigned i = 0; i < layerSize; i++)
    {
        delete individuals[i];
        individuals[i] = new_individual();
    }
}

void alps_population::open_new_layer()
{
    cout << "opening new layer" << endl;

    unsigned n = max_occupied_layer();
    // New layer opens up.
    curMaxLayer++;
    // Can't just realloc it.  :(
    individual** temp = new individual*[pop_size()];
    memset(temp, 0, sizeof(individual*) * pop_size());
    // Fill in the new layer offspring created from the previous layer.
    for (unsigned i = layer_start(n + 1); i <= layer_end(n + 1); i++)
    {
        temp[i] = tour_alps(n)->breed(tour_alps(n), antennasEA);
    }
    // Move the rest of old individuals into the new array.
    for (unsigned i = 0; i < layer_start(n + 1); i++)
    {
        temp[i] = individuals[i];
        individuals[i] = NULL;
    }
    delete[] individuals;
    individuals = temp;
}

void alps_population::promote_based_on_age()
{
    for (unsigned n = 0; n <= max_occupied_layer(); n++)
    {
        for (unsigned i = layer_start(n); i <= layer_end(n); i++)
        {
            if (n != maxLayer())
            {
                /*
                				if (individuals[i]->age == max_age(n) && (n + 1) > max_occupied_layer()) {
                					// Open up the new layer pre-emptively.  That way, these newly created
                					// individuals with have at least one generation to acquire a real fitness
                					// rating.
                					open_new_layer();
                				} else */
                if (individuals[i]->age >= max_age(n))
                {
                    individual* ind = individuals[i];
                    if ((n + 1) > max_occupied_layer())
                    {
                        // I don't expect this code branch to ever happen.  Will remove later.
                        //assert(0);
                        open_new_layer();
                    }
                    // It goes into the next layer.  How do I manage its departure?  Page 817.
                    int worst = worst_fitness(n + 1);
                    if (individuals[worst]->fitness >= individuals[i]->fitness)
                    {
                        // We bump out the worst in the next layer and keep this new one.
                        cout << "promoting individual " << i << " over " << worst << endl;
                        delete individuals[worst];
                        individuals[worst] = individuals[i];
                    }
                    else
                    {
                        // New individual can't hack it.  He'll be replaced.
                    }
                    // Who do we replace him with?  How about we breed a new one from the previous
                    // layer, or just make one up if he's from layer 0.
                    if (n == 0)
                    {
                        cout << "creating new individual for slot " << i << endl;
                        individuals[i] = new_individual();
                    }
                    else
                    {
                        cout << "breeding a new individual for slot " << i << endl;
                        individuals[i] = tour_alps(n - 1)->breed(tour_alps(n - 1), antennasEA);
                    }
                }
            }
        }
    }
}


// What's my current max layer dependent on the current generation?
unsigned alps_population::max_occupied_layer()
{
    return curMaxLayer;
    /*
    	unsigned i;
    	for (i = 0; i < layers; i++) {
    		if (generation <= max_age(i))
    			break;
    	}
    	// i is my current max layer WRT the current generation.
    	return min(i, layers - 1);
    */
}

unsigned alps_population::pop_size()
{
    return (max_occupied_layer() + 1) * layerSize;
}

unsigned alps_population::maxLayer()
{
    return layers - 1;
}

unsigned alps_population::layer_start(unsigned layer)
{
    return layer * layerSize;
}

unsigned alps_population::layer_end(unsigned layer)
{
    return layer_start(layer) + layerSize - 1;
}

unsigned alps_population::max_age(unsigned layer)
{
    return my_pow(2, layer) * ageGap;
}

void alps_population::select(void)
{
    // Deal with individuals who have maxed out of their layers first.
    promote_based_on_age();
    // Reseed first layer if needed.
    if (generation % ageGap == 0 && generation != 0)
    {
        // I don't need to reseed a new layer at all.  Not explicitly.
        //seed_new_layer0();
    }
    check_ages();

    individual** temp = new individual*[pop_size()];
    memset(temp, 0, sizeof(individual*) * pop_size());

    rank();
    for (unsigned layer = 0; layer <= max_occupied_layer(); layer++)
    {
        // Handle elitism.
        for (unsigned i = layer_start(layer); i < layer_start(layer) + elitism; i++)
        {
            temp[i] = individuals[i]->clone(true, antennasEA);
            temp[i]->age++;
        }
        for (unsigned ii = layer_start(layer) + elitism; ii <= layer_end(layer); ii++)
        {
            if (randf(0, 1.0f) < recombination)
            {
                temp[ii] = tour_alps(layer)->breed(tour_alps(layer), antennasEA);
            }
            else
            {
                temp[ii] = tour_alps(layer)->mutate(mutation, antennasEA);
            }
        }
    }
#if NET_ENVIRON
    for (unsigned ii = 0; ii < pop_size(); ii++) delete individuals[ii];
#else
    for (ii = 0; ii < pop_size(); ii++) delete individuals[ii];
#endif
    delete[] individuals;
    individuals = temp;
    generation++;
}

void alps_population::evaluate(unsigned generation)
{
    // Uncomment next line to do nothing.  DEBUG.
    population::evaluate(generation);
}


// Does an ALPS tournment for layer n.
individual* alps_population::tour_alps(unsigned n)
{
    assert(n >= 0);
    assert(n < layers);

    //int lstart = max((n - 1) * layerSize, 0);  // I don't know why this wasn't working.
    int lstart = (n - 1) * layerSize;
    if (lstart < 0) lstart = 0;
    int lend = (n + 1) * layerSize - 1;
    int tourSize = tournamentSize;
    individual*  best   = NULL;

    for (int i = 0; i < tourSize; i++)
    {
        individual* temp = individuals[rand() % (lend - lstart + 1) + lstart];
        if (! best || temp->fitness < best->fitness)
        {
            best = temp;
        }
    }
    return best;
}

#endif

//AJ
#if 0
int compare_descend_fitness(const void* a, const void* b)
{
    // this funky cast is required by qsort for arrays of pointers
    individual* pa = *(individual**) a;
    individual* pb = *(individual**) b;

    if (pb->fitness < pa->fitness)
        return 1;
    else if (pa->fitness < pb->fitness)
        return -1;
    return 0;
}
#endif
inline int my_pow(int base, int exponent)
{
    int result = 1;
    for (int i = 0; i < exponent; i++)
    {
        result *= base;
    }
    return result;
}

// Return the most fit individual from the population.
//AJ
/*tbll*/
#if 0
ostream& operator<<(ostream& out, population& pop)
{
    for (unsigned i = 0; i < pop.pop_size(); i++)
    {
        out << setw(3) << right << i << " " << setw(5) << right << pop.individuals[i]->age << " "<< setw(8) << right << pop.individuals[i]->fitness << endl;
    }

    return out;
}

ostream& operator<<(ostream& out, alps_population& pop)
{
    for (unsigned layer = 0; layer <= pop.max_occupied_layer(); layer++)
    {
        out << "Layer " << layer << endl;
        for (unsigned i = pop.layer_start(layer); i < pop.layer_start(layer + 1); i++)
        {
            out << setw(3) << right << i << " " << setw(5) << right << pop.individuals[i]->age << " "<< setw(8) << right << pop.individuals[i]->fitness << endl;
        }
    }
    return out;
}
#endif