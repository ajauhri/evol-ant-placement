#if 0
#ifndef ALGORITHM_HPP_INCLUDED
#define ALGORITHM_HPP_INCLUDED
#include "rapidxml/rapidxml.hpp"
#include "ant_config.hpp"
#include "ancillary_config.hpp"
#include "individual.hpp"
#include<string>
#include<vector>

/**
 * Base class for all implementations of stochastic algorithms
 */
class algorithm
{
	private:	

	protected:
		/* common data members for all algorithms */
		bool auto_seed;
		unsigned long int seed;
		float	exp_weight;
		float mutation_probability;

		std::string vehicle_model;

		/* everything to do with lua file provided as input */
		std::string aapot_file;
		ant_config_ptr clone(ant_config_ptr);

	public:
		algorithm(std::string, std::string);
		~algorithm(void);

		virtual void seek_algo_node();
		virtual void setup_ancillary_nodes();

		virtual void setup_run_context();
		virtual void setup_algo_params();
		virtual void setup_ant_placements();
		virtual void read_free_space_patterns();
		virtual void run() = 0;
		void run_simulation();	

		/* functions which work on an individual's data set */
		individual_ptr setup_individual();
		void write_to_file(individual_ptr, std::string);
		void evaluate_ant_config(target&);
		bool overlaps_ant(individual_ptr, position_ptr);

		/* stochastic operators */
		std::vector<individual_ptr> breed(individual_ptr, individual_ptr);
		void simple_mutation(individual_ptr);

		static int rand_integer(int, int);
		float cal_totaldb(float, float);
		void print_individual(individual_ptr);
};

#endif
#endif
