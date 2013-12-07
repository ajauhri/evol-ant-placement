#ifndef ALGORITHM_HPP_INCLUDED
#define ALGORITHM_HPP_INCLUDED
#include<ant_config.hpp>
#include<ancillary_config.hpp>
#include<individual.hpp>
#include<evaluation.hpp>
#include<string>
#include<vector>
/**
 * Base class for all implementations of stochastic algorithms
 */
class algorithm
{
    private:	
        std::vector<wire_ptr> load_wires(const std::string&, const std::string&); //called from load_wires, silly?
        void create_nec_strs();

    protected:
        /* common data members for all algorithms */
        bool auto_seed;
        float	exp_weight;
        float mutation;
        std::string lua_file;
        std::vector<std::string> plat_str_nec;

        //ant_config_ptr clone(ant_config_ptr);
        unsigned int min_theta = 0;
        float step_theta = 46;
        float incr_theta = 4;
        float min_phi = 0;
        float step_phi = 90;
        float incr_phi = 4;
        float min_freq = 100;
        unsigned int step_freq = 1;
        float incr_freq = 10;

        unsigned int num_polar(void);
        virtual void write_platform(std::ofstream&);
        virtual void write_ant(std::ofstream&, ant_config_ptr&, unsigned int, unsigned int);
        

    public:
        algorithm(std::string);
        ~algorithm(void);

        std::vector<ant_config_ptr> ant_configs; //stores antennas positions and all wires mentioned in the nec file
        ant_config_ptr platform;


        std::vector<individual_ptr> free_inds; //free space indivuals to compare against

        virtual void setup_run_context();
        virtual void setup_algo_params();
        virtual void setup_ant_placements();
        virtual void write_freespace();
        virtual void load_nec_files();
        virtual void run_freespace();
        virtual void read_freespace();
        virtual unsigned int read_nou(const std::string, const evaluation_ptr &);
        virtual void run() = 0;

        //virtual void seek_algo_node();
#if 0
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
#endif
};

#endif
