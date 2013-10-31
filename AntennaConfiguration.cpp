#include<math.h>
#include<iostream>
#include<assert.h>
#include<set>
#include<float.h>
#include<random>
#include "stdafx.h"
#include "AntennaConfiguration.h"
#include "Geometry.h"
#include "StatusMessages.h"
#include "APOT6Dlg.h"
#include "Algorithms.h"
#include "SA.h"
#include "HC.h"
#include "EvolutionaryStrategy.h"
#include<algorithm>
EAParams		EAInfo;

NECSimulator*	Simulator;

BOOL			SearchThreadRunning = FALSE;
BOOL			SearchAbortThread = FALSE;
BOOL			RunLocal = FALSE;
//AJ
#if 0
vector<antenna_individual*>	outputs;
#endif

vector<antenna_configuration*> outputs;
CString gUserName("apot");
CString	gPassword("nec");

bool 
GAparams_valid(void)
{
    return ((EAInfo.generations > 0)
            && (EAInfo.popSize > 0)
            && (EAInfo.mutation >= 0.0f && EAInfo.mutation <= 1.0f)
            && gUserName == EAInfo.username && gPassword == EAInfo.password);
}

bool 
SAparams_valid(void)
{
    return ((SAInfo.iterations > 0)
            && (SAInfo.mutationProbability <= 1.0f)
            && (SAInfo.initialTemp >= 0)
            && (SAInfo.tempFactor > 0.0f)
            && (SAInfo.iterationsPerTempChng > 0)
            && (SAInfo.outputFreq > 0)
            && (SAInfo.convergenceFactor > 0)
            && (SAInfo.genCondition >= 0)
            && (SAInfo.fitnessCondition >= 0)
            && gUserName == EAInfo.username && gPassword == EAInfo.password);
}

bool 
HCparams_valid(void)
{
    return ((HCInfo.iterations > 0)
            && (HCInfo.mutationProbability <= 1.0f)
            && (HCInfo.outputFreq > 0)
            && (HCInfo.convergenceFactor > 0)
            && (HCInfo.genCondition >= 0)
            && (HCInfo.fitnessCondition >= 0)
            && gUserName == EAInfo.username && gPassword == EAInfo.password);
}

bool 
ESparams_valid(void)
{
    return ((ESInfo.generations > 0)
            && (ESInfo.mutation_probability >= 0.0f && ESInfo.mutation_probability <= 1.0f)
            && (ESInfo.mu < ESInfo.lambda)
            && gUserName == EAInfo.username && gPassword == EAInfo.password);
}



//AJ
#if 0
int compare_descend_fitness(const void* a, const void* b);
#endif


/*abaj*/
int 
rand_integer(int min_value, 
			int max_value)
{
    return (int) rand() % max_value;
    //return (int) ((double)rand() / ((double) RAND_MAX + 1) * (max_value - min_value));
}

double 
rand_double()
{
    return ((double)rand()/(double)RAND_MAX);
}



/*abaj*/
/*tbll*/
antenna_configuration::antenna_configuration(void)
{
    fitness = 0.0f;
    age = 0;
}

/*abaj*/
antenna_configuration::~antenna_configuration(void)
{

    ants.clear();
    results.clear();
}

/*abaj*/
void 
antenna_configuration::initialize(vector<antenna*> antennas)
{
    for (unsigned int ii = 0; ii < antennas.size(); ii++)
    {
        antenna* ant = new antenna;
        strcpy(ant->label, antennas[ii]->label);

        /* Initialize all wires for an antenna. The offset value w.r.t. to the platform will be evaluated at the creation of the nec decks. */
        for (unsigned int jj = 0; jj < antennas[ii]->wires.size(); jj++)
        {
            wire	wr(*(antennas[ii]->wires[jj]));
            ant->wires.push_back(new wire(wr));
        }
        ant->params = (analysis_params) antennas[ii]->params;

        //Randomly select an antenna placement from allowable positions
        int antenna_placement = rand_integer(0,antennas[ii]->points->size());//(int) ((double)rand() / ((double)RAND_MAX+1) * antennas[ii]->points->size());
        point pt(*(antennas[ii]->points->at(antenna_placement)));
        ant->points->push_back(new point(pt));
        ant->color = antennas[ii]->color;
        ants.push_back(ant);
    }
    age = 0;
}

/*abaj*/
void 
antenna_configuration::setup(unsigned int id, 
							signed int generation)
{
    char infile[100];

    if(generation != -1)
    {
        for (unsigned ii = 0; ii < ants.size(); ii++)
        {
            sprintf(infile, "./GEN%04d/I%09dA%02d.nec", generation, id, ii);
            write(infile, ii);
            //sprintf(outfile, "./GEN%04d/I%09dA%02d.nou", generation, id, ii);
        }

		/* to measure coupling an extra nec file is required */
		//sprintf(infile, "./GEN%04d/I%09dA%02d.nec", generation, id, ants.size());
		//write_coupling(infile, ants.size());
    }

    else if(generation == -1)
    {
        for (unsigned ii = 0; ii < ants.size(); ii++)
        {
            sprintf(infile, "./Iterations/I%06dA%02d.nec", id, ii);
            write(infile, ii);
            //sprintf(outfile, "./GEN%04d/I%09dA%02d.nou", generation, id, ii);
        }
		/* to measure coupling an extra nec file is required */
		//sprintf(infile, "./Iterations/I%06dA%02d.nec", id, ants.size());
		//write_coupling(infile, ants.size());
    }
}

/*abaj*/
void 
antenna_configuration::write(const char* filename, 
							unsigned int which) const
{
    int excitation;
    point offset;

    assert(which < ants.size());
    //	build the nec deck
    clear_nec_deck();
    platform->translate2deck(offset);

    // convert genotype into an antenna offset
    for (unsigned int ii = 0; ii < ants.size(); ii++)
    {
        if (which == ii) excitation = g_nec_segment;
        ants[ii]->translate2deck(*(ants[ii]->points->at(0)));
    }
    ants[which]->append_analysis(excitation);
    print_nec_deck(filename);
}

/*for the CP card*/
void 
antenna_configuration::write_coupling(const char* filename, 
							unsigned int id) const
{
    vector<int> *antenna_start_tags = new vector<int>[2];
	memset(antenna_start_tags, 0, sizeof(vector<int>) * 2);
    point offset;

    //	build the nec deck
    clear_nec_deck();
    platform->translate2deck(offset);
	// convert genotype into an antenna offset
    for (unsigned int ii = 0; ii < ants.size(); ii++)
    {
		antenna_start_tags->push_back(g_nec_segment);
        ants[ii]->translate2deck(*(ants[ii]->points->at(0)));
    }

	for (unsigned int i = 0; i < ants.size(); i++)
	{
		ants[i]->append_coupling_cards(antenna_start_tags, i);
	}
	print_nec_deck(filename);
}



/*abaj*/
// Read back the nou file.
void 
antenna_configuration::evaluate(unsigned int id, 
								vector<antenna*> antennas,
								float weight, 
								signed int generation)
{
    results.clear();
    fitness = 0;

    float worst = 0.0f;
    char *buffer = new char[100];
    if (antennas_overlap())
    {
        statusf("warning: individual %u is rejected overlapping antennas.", id);
        fitness = WORST_FITNESS;
        return;
    }

    if (generation != -1)
    {
        for (unsigned int ii = 0; ii < ants.size(); ii++)
        {
			try 
			{
				evaluation *eval = new evaluation;
				memset(eval, 0, sizeof(eval));

				unsigned int count = ants[ii]->num_polar() * ants[ii]->params.step_freq;
				sprintf(buffer, "./GEN%04d/I%09dA%02d.nou", generation, id, ii);
				unsigned int read = read_nec_results(buffer, *eval, *(ants[ii]));
				if (read != count)
				{
					delete eval;
					statusf(READ_PROBLEM, generation);
					::Sleep(1000);
					for (unsigned int kk = 0; kk < results.size(); kk++)
					    delete results[kk];
					results.clear();
	                fitness = WORST_FITNESS;
		            return;
			    }
				results.push_back(eval);
				one_antenna_on_fitness.push_back(evaluation::compare(*eval, antennas[ii]->results, weight));
	            fitness += one_antenna_on_fitness[ii];
				
				/*tbll*/
			    if (fitness > worst) worst = fitness;
				}
			catch (CMemoryException *e) 
			{
				e->Delete();
			}

        }
		/* read coupling results */
		//sprintf(buffer, "./GEN%04d/I%09dA%02d.nou", generation, id, ants.size());
		//fitness = read_coupling_results(buffer, ants.size());
    }

    else if (generation == -1)
    {
        for (unsigned int ii = 0; ii < ants.size(); ii++)
        {
            evaluation* eval = new evaluation;
			memset(eval, 0, sizeof(eval));
            unsigned int count = ants[ii]->num_polar() * ants[ii]->params.step_freq;
            sprintf(buffer, "./Iterations/I%06dA%02d.nou", id, ii);
            unsigned int read = read_nec_results(buffer, *eval, *(ants[ii]));
            if (read != count)
            {
                delete eval;
                statusf(READ_PROBLEM, id);
                ::Sleep(1000);

                for (unsigned int kk = 0; kk < results.size(); kk++)
                    delete results[kk];
                results.clear();
                fitness = WORST_FITNESS;
                return;
            }
            results.push_back(eval);
            one_antenna_on_fitness.push_back(evaluation::compare(*eval, antennas[ii]->results, weight));
            fitness += one_antenna_on_fitness[ii];
            /*tbll*/
            if (fitness > worst) worst = fitness;
        }
    }
	delete[] buffer;
}



/*abaj*/
// Returns true if any of the antennas are in the same location.
bool 
antenna_configuration::antennas_overlap()
{
    for (unsigned i = 0; i < ants.size(); i++)
    {
        point* a = ants[i]->points->at(0);
        for (unsigned j = i + 1; j < ants.size(); j++)
        {
            point* b = ants[j]->points->at(0);
            if (*a == *b)
                return true;
        }
    }
    return false;
}


antenna_configuration::antenna_configuration(const antenna_configuration& other)
{
    for(int i = 0; i<other.ants.size(); i++)
        ants.push_back(new antenna(*other.ants.at(i)));
}

bool 
antenna_configuration::check_overlap(point* b)
{
    for (unsigned int i = 0; i < ants.size(); i++)
    {
        point *a = ants[i]->points->at(0);
        if(*a == *b)
            return true;
    }
    return false;
}



/*abaj*/
antenna_configuration* 
antenna_configuration::clone(bool deep) const
{
    antenna_configuration *clone = new antenna_configuration;
    assert(clone->results.size() == 0);

    for (unsigned int ii = 0; ii<ants.size(); ii++)
        clone->ants.push_back(new antenna(*(ants[ii])));

    if (deep)
    {
        for (unsigned int ii = 0; ii < results.size(); ii++)
        {
            clone->results.push_back(new evaluation(*(results[ii])));
            clone->one_antenna_on_fitness.push_back(one_antenna_on_fitness[ii]);
        }
        clone->fitness = fitness;
    }
    /*tbll*/
    //clone->age = age;
    return clone;
}


/*abaj*/
vector<antenna_configuration*> antenna_configuration::breed(const antenna_configuration* mate)
{
	vector<antenna_configuration*> children;
	children.push_back(mate->clone(false));
	children.push_back(this->clone(false));
	//antenna_configuration* matey = (antenna_configuration*) mate->clone(false);
    //antenna_configuration* child = (antenna_configuration*) this->clone(false);
    /*tbll Should be zero */
    //child->fitness = WORST_FITNESS;
    /*tbll - What is index representation?*/
    //if (EAInfo.representation == INDEX_REP) {
    int xover = rand_integer(0, ants.size());

    for (unsigned ii = xover; ii < ants.size(); ii++)
    {
		children[0]->ants[ii] = new antenna(*(this->ants[ii]));
		children[1]->ants[ii] = new antenna(*(mate->ants[ii]));
    }


    /*tbll*/
#if 0
    else   //if (EAInfo.representation == SURFACE_REP) {
    {
        // SURFACE_REP
        // XXX - Here we're breeding all the floats.  Is that advisable? -SEC
        for (unsigned ii = 0; ii < antennas.size(); ii++)
        {
            child->surface[ii].x = breed_floats(matey->surface[ii].x, surface[ii].x);
            child->surface[ii].y = breed_floats(matey->surface[ii].y, surface[ii].y);
        }
    }
#endif

    /*tbll*/
    // The breed used to do a mutation.  We're going to separate these two operations.
    children.at(0)->age = max(this->age, mate->age);
	children.at(1)->age = max(this->age, mate->age);
    return children;
}


antenna_configuration* 
antenna_configuration::simple_mutation(float mutation,
										vector<antenna*> antennas)
{
    antenna_configuration* child = (antenna_configuration*) this->clone(false);
    /*tbll*/
    //child->fitness = WORST_FITNESS;
#if NET_ENVIRON
    for (unsigned int ii = 0; ii < ants.size(); ii++)
    {
        // mutation
#else
    for (ii = 0; ii < antennas.size(); ii++)    // mutation
    {
#endif
        if (randf(0, 1.0f) < mutation)
        {
            if (EAInfo.representation == INDEX_REP || selected_algorithm != EVOLUTIONARY_ALGORITHM)
            {
                int random_integer = rand_integer(0, antennas[ii]->points->size());
                child->ants[ii]->points->pop_back();
                point pt(*(antennas[ii]->points->at(random_integer)));
                child->ants[ii]->points->push_back(new point(pt));
            }
        }
    }
    return child;
}


/*abaj*/
antenna_configuration* 
antenna_configuration::global_neighbourhood_mutation(float mutation, 
											vector<antenna*> antennas)
{
	antenna_configuration *child = (antenna_configuration*) this->clone(false);
	for (int i = 0; i < ants.size(); i++) 
	{
		if (randf(0, 1.0f) < mutation)
		{
			point *antenna_point = child->ants.at(i)->points->at(0);			
			vector<int> neighbours;
			get_global_neighbourhood(antenna_point, antennas[i]->points, &neighbours);
			int random_integer = rand_integer(0, neighbours.size());
            child->ants[i]->points->pop_back();
			point pt(*(antennas[i]->points->at(neighbours.at(random_integer))));
            child->ants[i]->points->push_back(new point(pt));
			neighbours.clear();
		}
	}
	return child;
}



/*Based on the distance of the antenna_point from the centroid, 
a list of of neighbours is created. If their are no neighbours,
10% of the closest valid points are added to the list of neighbours */
void
antenna_configuration::get_global_neighbourhood(point *antenna_point, 
										vector<point*> *valid_points, 
										vector<int> *neighbours)
{
	vector<ValidPoint*> points;
	for (int i = 0; i < valid_points->size(); i++)
			{
				ValidPoint *p = new ValidPoint();
				float distance = valid_points->at(i)->distance(*antenna_point);
				if ( distance <= antenna_point->dist_from_centroid )// && distance != 0.0f)
					neighbours->push_back(i);
				p->distance = distance;
				p->id = i;
				points.push_back(p);
			}

	/* if antenna_point is the centroid itself, and there are no other valid points within the neighbourhood */
	if (neighbours->size() == 1) 
	{
		std::sort(points.begin(), points.end(), distance_sort);

		for (int i = 0; i < points.size() * (0.1); i++)
			neighbours->push_back(points.at(i)->id);
	}

	for (int i = 0; i < points.size(); i++) delete points[i];
	points.clear();
}



/*abaj*/
antenna_configuration* 
antenna_configuration::local_neighbourhood_mutation(float mutation, 
											vector<antenna*> antennas)
{
	antenna_configuration *child = (antenna_configuration*) this->clone(false);
	for (int i = 0; i < ants.size(); i++) 
	{
		if (randf(0, 1.0f) < mutation)
		{
			point *antenna_point = child->ants.at(i)->points->at(0);
			vector<int> neighbours;
			get_local_neighbourhood(antenna_point, antennas[i]->points, &neighbours);
			int random_integer = rand_integer(0, neighbours.size());
            child->ants[i]->points->pop_back();
			point pt(*(antennas[i]->points->at(neighbours.at(random_integer))));
            child->ants[i]->points->push_back(new point(pt));
			neighbours.clear();
		}
	}
	return child;
}

void
antenna_configuration::get_local_neighbourhood(point *antenna_point, 
										vector<point*> *valid_points, 
										vector<int> *neighbours)
{
	vector<ValidPoint*> points;
	for (int i = 0; i < valid_points->size(); i++)
			{
				ValidPoint *p = new ValidPoint();
				float distance = valid_points->at(i)->distance(*antenna_point);
				if ( distance <= mesh_resolution )// && distance != 0.0f)
					neighbours->push_back(i);
				p->distance = distance;
				p->id = i;
				points.push_back(p);
			}

	if (neighbours->size() == 1) 
	{
		std::sort(points.begin(), points.end(), distance_sort);
		for (int i = 0; i < points.size() * (0.1); i++)
			neighbours->push_back(points.at(i)->id);
	}
	for (int i = 0; i < points.size(); i++) delete points[i];
	points.clear();
}


int
distance_sort(ValidPoint *p1, ValidPoint *p2)
{
    return p1->distance < p2->distance;
}

/*abaj*/
void 
antenna_configuration::simulate(NECSimulator* sim,
								unsigned int id,
								signed int generation)
{
    char infile[100];
    char outfile[100];
    if (generation != -1)
    {
        for (unsigned ii = 0; ii < ants.size(); ii++)
        {
            sprintf(infile, "./GEN%04d/I%09dA%02d.nec", generation, id, ii);
            sprintf(outfile, "./GEN%04d/I%09dA%02d.nou", generation, id, ii);
            sim->RunIndividual(infile, outfile);
            sim->Retrieve(generation);
        }
    }
    else if(generation == -1)
    {
        for (unsigned ii = 0; ii < ants.size(); ii++)
        {
            sprintf(infile, "./Iterations/I%06dA%02d.nec", id, ii);
            sprintf(outfile, "./Iterations/I%06dA%02d.nou", id, ii);
            sim->RunIndividual(infile, outfile);
            sim->Retrieve();
        }
    }
}


/*abaj*/
/*tbll with reference to the call being made in OpenGLView*/
// Returns the point of the antenna on the platform.
point* 
antenna_configuration::map_point(unsigned antenna_index, 
								vector<antenna*> antennas) const
{
    return antennas[antenna_index]->points->at(0);
}

/*abaj*/
void 
antenna_configuration::free_display_lists()
{
    for (unsigned int ii = 0; ii < results.size(); ii++)
    {
        evaluation* eval = results[ii];
        int count = eval->radiation.size();
        for (int jj = 0; jj < count; jj++)
        {
            displayhandle* dp = eval->radiation[jj]->dlist;
            if (dp != NULL)
            {
                dp->count--;
                if (dp->count == 0) delete dp;
                eval->radiation[jj]->dlist = NULL;
            }
        }
    }
}


//AJ
#if 0
antenna_individual::antenna_individual(void):individual()
{
    coords = NULL;
    ant = NULL;
}

antenna_individual::~antenna_individual(void)
{
    if (coords != NULL) delete[] coords;
    for (unsigned ii = 0; ii < results.size(); ii++) delete results[ii];
    if (ant != NULL) delete ant;
}

void antenna_individual::initialize(vector<antenna*> antennas)
{
    coords = new unsigned[antennas.size()];
    surface = new struct surface [antennas.size()];

    for (unsigned ii = 0; ii < antennas.size(); ii++)
    {
        coords[ii]	= rand();
        surface[ii].x = randf(0.0, 1.0);
        surface[ii].y = randf(0.0, 1.0);
    }

#if NET_ENVIRON
    for (unsigned ii = 0; ii < results.size(); ii++) delete results[ii];
#else
    for (ii = 0; ii < results.size(); ii++) delete results[ii];
#endif
    results.clear();
    assert(results.empty());
    fitness = WORST_FITNESS;
}

// We want to map every combination of antenna positions to a number.
unsigned antenna_individual::to_number(vector<antenna*> antennas)
{
    if (EAInfo.representation == SURFACE_REP)
    {
        surface_to_coords(antennasEA);
    }
    unsigned last_n = 1;
    unsigned result = 0;

    // n_0 = 1
    // \Sum_{i = 1} a_i * n_{i - 1}
    // n_i = equals the count of antenna positions for antenna i.
    for (unsigned i = 0; i < antennas.size(); i++)
    {
        unsigned n = antennas[i]->points->size();
        unsigned a = coords[i] % n;
        result += a * last_n;
        last_n = n;
    }
    return result;
}

void antenna_individual::from_number(unsigned r, vector<antenna*> antennas)
{
    for (unsigned i = 0; i < antennas.size(); i++)
    {
        unsigned n = antennas[i]->points->size();
        coords[i] = r % n;
        r /= n;
    }
}

unsigned antenna_individual::max_number(vector<antenna*> antennas)
{
    unsigned r = 1;
    for (unsigned i = 0; i < antennas.size(); i++)
    {
        r *= antennas[i]->points->size();
    }
    return r;
}

// Write out the nec file.
void antenna_individual::setup(unsigned int id, vector<antenna*> antennas, signed int generation)
{
    char infile[100];
    // convert genotype into an antenna offset
    /* generation != -1 for GA ONLY */
    if(generation!=-1)
    {
        for (unsigned ii = 0; ii < antennas.size(); ii++)
        {
            sprintf(infile, "./GEN%04d/I%09dA%02d.nec", generation, id, ii);
            write(infile, ii, antennas);
            //sprintf(outfile, "./GEN%04d/I%09dA%02d.nou", generation, id, ii);
        }
    }
    else if(generation == -1)
    {
        for (unsigned ii = 0; ii < antennas.size(); ii++)
        {
            sprintf(infile, "./Iterations/I%06dA%02d.nec", id, ii);
            write(infile, ii, antennas);
            //sprintf(outfile, "./GEN%04d/I%09dA%02d.nou", generation, id, ii);
        }
    }

}

void antenna_individual::simulate(NECSimulator* sim, unsigned id, vector<antenna*> antennas, signed generation)
{
    char infile[100];
    char outfile[100];
    if (generation != -1)
    {
        for (unsigned ii = 0; ii < antennas.size(); ii++)
        {
            sprintf(infile, "./GEN%04d/I%09dA%02d.nec", generation, id, ii);
            sprintf(outfile, "./GEN%04d/I%09dA%02d.nou", generation, id, ii);
            sim->RunIndividual(infile, outfile);
            sim->Retrieve(generation);
        }
    }
    else if(generation == -1)
    {
        for (unsigned ii = 0; ii < antennas.size(); ii++)
        {
            sprintf(infile, "./Iterations/I%06dA%02d.nec", id, ii);
            sprintf(outfile, "./Iterations/I%06dA%02d.nou", id, ii);
            sim->RunIndividual(infile, outfile);
            sim->Retrieve();
        }
    }
}

// Read back the nou file.
void antenna_individual::evaluate(unsigned int id, vector<antenna*> antennas, float weight, signed int generation)
{
    results.clear();

    float worst = 0.0f;
    char buffer[100];
    if (antennas_overlap(antennas))
    {
        statusf("warning: individual %u is rejected overlapping antennas.", id);
        fitness = WORST_FITNESS;
        return;
    }

    if (generation != -1)
    {

        for (unsigned int ii = 0; ii < antennas.size(); ii++)
        {

            evaluation* eval = new evaluation;
            unsigned int count = antennas[ii]->num_polar() * antennas[ii]->params.step_freq;

            sprintf(buffer, "./GEN%04d/I%09dA%02d.nou", generation, id, ii);
            unsigned int read = read_nec_results(buffer, *eval, *(antennas[ii]));
            if (read != count)
            {
                delete eval;
                statusf(READ_PROBLEM, generation);
                ::Sleep(1000);
                for (unsigned int kk = 0; kk < results.size(); kk++)
                {
                    delete results[kk];
                }
                results.clear();
                fitness = WORST_FITNESS;
                return;
            }
            results.push_back(eval);

            float fit = evaluation::compare(*eval, antennas[ii]->results, weight);
            if (fit > worst) worst = fit;

            fitness = worst;
        }
    }

    else if (generation == -1)
    {

        for (unsigned int ii = 0; ii < antennas.size(); ii++)
        {

            evaluation* eval = new evaluation;
            unsigned int count = antennas[ii]->num_polar() * antennas[ii]->params.step_freq;

            sprintf(buffer, "./Iterations/I%06dA%02d.nou", id, ii);
            unsigned int read = read_nec_results(buffer, *eval, *(antennas[ii]));
            if (read != count)
            {
                delete eval;
                statusf(READ_PROBLEM, id);
                ::Sleep(1000);

                for (unsigned int kk = 0; kk < results.size(); kk++)
                {
                    delete results[kk];
                }
                results.clear();
                fitness = WORST_FITNESS;
                return;
            }
            results.push_back(eval);
            float fit = evaluation::compare(*eval, antennas[ii]->results, weight);
            if (fit > worst) worst = fit;
            fitness = worst;
        }
    }

}

// Maps the surface onto the coords.
void antenna_individual::surface_to_coords(vector<antenna*> antennas)
{
    for (unsigned i = 0; i < antennas.size(); i++)
    {
        unsigned antenna_index = i;
        unsigned idx;
        // We want to map from the surface x,y in [0, 1] to the surface on the points.
        // Problem: map [0,1] to a point on the platform, then find the closest point
        // that was selected for antenna placement.
        point min = platform->min_point();
        point max = platform->max_point();
        float x = surface[antenna_index].x * (max.x - min.x) + min.x;
        float y = surface[antenna_index].y * (max.y - min.y) + min.y;
        point p(x,y,0);
        float min_d2 = FLT_MAX; // Distance squared
        for (unsigned j = 0; j < antennas[antenna_index]->points->size(); j++)
        {
            point *r = antennas[antenna_index]->points->at(j);
            p.z = r->z; // We're ignoring z.
            float d2 = r->distance2(p);
            if (d2 < min_d2)
            {
                idx = j;
                min_d2 = d2;
            }
        }
        coords[antenna_index] = idx;
    }
}

// Returns the point of the antenna on the platform.
point* antenna_individual::map_point(unsigned antenna_index, vector<antenna*> antennas) const
{
    int idx;
    assert(antenna_index < antennas.size());
    if (EAInfo.representation == INDEX_REP || selected_algorithm!= EVOLUTIONARY_ALGORITHM)
    {
        idx = coords[antenna_index] % antennas[antenna_index]->points->size();
    }
    else
    {
        // We want to map from the surface x,y in [0, 1] to the surface on the points.
        // Problem: map [0,1] to a point on the platform, then find the closest point
        // that was selected for antenna placement.
        point min = platform->min_point();
        point max = platform->max_point();
        float x = surface[antenna_index].x * (max.x - min.x) + min.x;
        float y = surface[antenna_index].y * (max.y - min.y) + min.y;
        point p(x,y,0);
        float min_d2 = FLT_MAX; // Distance squared
        for (unsigned j = 0; j < antennas[antenna_index]->points->size(); j++)
        {
            point *r = antennas[antenna_index]->points->at(j);
            p.z = r->z; // We're ignoring z.
            float d2 = r->distance2(p);
            if (d2 < min_d2)
            {
                idx = j;
                min_d2 = d2;
            }
        }
    }
    return antennas[antenna_index]->points->at(idx);
}

void antenna_individual::write(const char* filename, unsigned int which, vector<antenna*> antennas) const
{
    int excitation;
    point offset;

    assert(which < antennas.size());
    //	build the nec deck
    clear_nec_deck();
    platform->translate2deck(offset);
    // convert genotype into an antenna offset
    for (unsigned int ii = 0; ii < antennas.size(); ii++)
    {

        if (which == ii) excitation = g_nec_segment;
        antennas[ii]->translate2deck(*map_point(ii, antennas));
    }
    antennas[which]->append_analysis(excitation);
    print_nec_deck(filename);
}

void antenna_individual::print( vector<antenna*> antennas, bool all) const
{
    cout << "F = " << fitness << "\t";
// KRAUS TBD
    if (all)
    {
        printf("< ");
        for (unsigned int ii = 0; ii < antennas.size(); ii++)
        {
            printf("%d ", coords[ii]);
        }
        printf(">");
    }
    cout << endl;
}

individual* antenna_individual::clone(bool deep, vector<antenna*> antennas) const
{
    antenna_individual* clone = new antenna_individual;
    clone->initialize(antennas);
    clone->fitness = fitness;
    clone->age = age;
    for (unsigned int ii = 0; ii < antennas.size(); ii++)
    {
        clone->coords[ii] = coords[ii];
        clone->surface[ii].x = surface[ii].x;
        clone->surface[ii].y = surface[ii].y;
    }
    assert(clone->results.size() == 0);
    if (deep)
    {
        for (unsigned int ii = 0; ii < results.size(); ii++)
        {
            clone->results.push_back(new evaluation(*(results[ii])));
        }
    }
    if (ant != NULL) clone->ant = new antenna(*ant);
    return clone;
}


individual* antenna_individual::breed(const individual* mate, vector<antenna*> antennas)
{
    antenna_individual* matey = (antenna_individual*) mate;
    antenna_individual* child = (antenna_individual*) this->clone(false, antennas);
    child->fitness = WORST_FITNESS;
    if (EAInfo.representation == INDEX_REP)
    {
        int xover = rand() % antennas.size();
        for (unsigned ii = xover; ii < antennas.size(); ii++)
        {
            child->coords[ii] = matey->coords[ii];
        }
    }
    else     //if (EAInfo.representation == SURFACE_REP) {
    {
        // SURFACE_REP
        // XXX - Here we're breeding all the floats.  Is that advisable? -SEC
        for (unsigned ii = 0; ii < antennas.size(); ii++)
        {
            child->surface[ii].x = breed_floats(matey->surface[ii].x, surface[ii].x);
            child->surface[ii].y = breed_floats(matey->surface[ii].y, surface[ii].y);
        }
    }

    // The breed used to do a mutation.  We're going to separate these two operations.
    child->age = max(this->age, mate->age) + 1;
    return child;
}

individual* antenna_individual::mutate(float mutation, vector<antenna*> antennas)
{
    antenna_individual* child = (antenna_individual*) this->clone(false, antennas);
    child->fitness = WORST_FITNESS;
#if NET_ENVIRON
    for (unsigned int ii = 0; ii < antennas.size(); ii++)    // mutation
    {
#else
    for (ii = 0; ii < antennas.size(); ii++)    // mutation
    {
#endif
        if (randf(0, 1.0f) < mutation)
        {
            if (EAInfo.representation == INDEX_REP || selected_algorithm!=EVOLUTIONARY_ALGORITHM)
            {
                child->coords[ii] = rand();
            }
            else     // if (EAInfo.representation == SURFACE_REP) {
            {
                child->surface[ii].x = bound(gaussf(child->surface[ii].x, MUTATION_STD), 0.0, 1.0);
                child->surface[ii].y = bound(gaussf(child->surface[ii].y, MUTATION_STD), 0.0, 1.0);
            }
        }
    }
    child->age++;
    return child;
}

void antenna_individual::free_display_lists()
{
    for (unsigned int ii = 0; ii < results.size(); ii++)
    {
        evaluation* eval = results[ii];
        int count = eval->radiation.size();
        for (int jj = 0; jj < count; jj++)
        {
            displayhandle* dp = eval->radiation[jj]->dlist;
            if (dp != NULL)
            {
                dp->count--;
                if (dp->count == 0) delete dp;
                eval->radiation[jj]->dlist = NULL;
            }
        }
    }
}

void antenna_individual::build_antenna(vector<antenna*> antennas)
{
    if (ant != NULL) delete ant;
    ant = new antenna(*platform);
    for (unsigned int ii = 0; ii < antennas.size(); ii++)
    {
        point pt(*(map_point(ii, antennas)));
        for (unsigned int jj = 0; jj < antennas[ii]->wires.size(); jj++)
        {
            wire	wr(*(antennas[ii]->wires[jj]));
            wr.offset(pt);
            ant->wires.push_back(new wire(wr));
        }
    }

    if (results.size() > 0)
    {
        for (unsigned int ii = 0; ii < antennas.size(); ii++)  		// for each antenna
        {

            int count = antennas[ii]->num_polar();
//			float freq = antennas[ii]->params.min_freq;
            for (unsigned int kk = 0; kk < antennas[ii]->params.step_freq; kk++)  	// for each frequency
            {

                assert(results[ii] != NULL);
                pattern* pat = results[ii]->radiation[kk];
                assert(pat != NULL);
                results[ii]->radiation[kk] = pat;
//				freq += antennas[ii]->params.incr_freq;
            }
        }
    }
}

// Returns true if any of the antennas are in the same location.
bool antenna_individual::antennas_overlap(vector<antenna*> antennas)
{
    for (unsigned i = 0; i < antennas.size(); i++)
    {
        point* a = map_point(i, antennas);
        for (unsigned j = i + 1; j < antennas.size(); j++)
        {
            point* b = map_point(j, antennas);
            if (*a == *b)
                return true;
        }
    }
    return false;
}

// Tries to fix an antenna overlap by mutating the current antenna.  Not guaranteed to work.
void antenna_individual::fix_antennas_overlap(vector<antenna*> antennas)
{
    unsigned tries = 10;

    for (unsigned j = 0; j < tries && antennas_overlap(antennas); j++)
    {
        //
    }
}

#endif

//AJ
#if 0
float breed_floats(float a, float b)
{
    if (a > b)
        return breed_floats(b, a);
    float d = b - a;
    float d2 = d/2;
    return randf(a - d2, b + d2);
}
#endif

#if 0
void sim_alps_population::initialize(int size)
{
    // Initialize from EAInfo.
    layers = EAInfo.layers;
    layerSize = EAInfo.layerSize;
    ageGap = EAInfo.ageGap;
    mutation = EAInfo.mutation;
    elitism = EAInfo.elitism;
    recombination = EAInfo.recombination;
    tournamentSize = EAInfo.tournamentSize;

    for (unsigned ii = 0; ii < outputs.size(); ii++) delete outputs[ii];
    outputs.clear();

    alps_population::initialize(size);
}


#include "SimPopulation.cpp"
#include "SimAlpsPopulation.cpp"

void exhaustive_search::initialize(int size)
{
    population::initialize(1);
    ind = (antenna_individual*) individuals[0];
    unsigned initial_generation = 14282; //22212; //9235;
    EAInfo.generations = ind->max_number(antennasEA) - initial_generation;
    ind->from_number(initial_generation,antennasEA); // Start at number zero.
    out[0] = fopen("fitness-index.data", "w");
    /*
    	out[1] = fopen("fitness-points.data", "w");
    	out[2] = fopen("fitness-surface.data", "w");
    */
}

void exhaustive_search::evaluate(unsigned generation)
{
    sim_population::evaluate(generation);
    // Before we change anything, let's write our results to a file for plotting later.
    for (unsigned i = 0; i < antennasEA.size(); i++)
    {
        fprintf(out[0], "%u ", ind->coords[i]);

        // XXX - This'll only work for an exhaustive search which has no bearing on
        // surface representation.
        /*
        		point* p = ind->map_point(i, antennas);
        		fprintf(out[1], "%f %f ", p->x, p->y);

        		fprintf(out[2], "%f %f ", ind->surface[i].x, ind->surface[i].y);
        */
    }
    for (int i = 0; i < MAX_OUT; i++)
    {
        fprintf(out[i], "%f\n", ind->fitness);
        fflush(out[i]);
    }
}

void exhaustive_search::select()
{
    // Go to the next individual.
    unsigned i = ind->to_number(antennasEA);
    ind->from_number(i + 1, antennasEA);
}

exhaustive_search::~exhaustive_search()
{
    // Close our stats files.
    for (int i = 0; i < MAX_OUT; i++)
        fclose(out[i]);
}
#endif