#ifndef ALGORITHM_HPP_INCLUDED
#define ALGORITHM_HPP_INCLUDED
#include<ant_config.hpp>
#include<individual.hpp>
#include<evaluation.hpp>
#include<string>
#include<vector>

/**
 * Base class for all implementations of evolutionary algorithms
 */
class algorithm
{
    private:	
        std::vector<wire_ptr> load_wires(const std::string&, const std::string&); //called from load_wires, silly?
        void create_nec_strs();

    protected:
        /* common data members for all algorithms */
        bool m_auto_seed;
        
        // flag to determine whether to clean and remove nec out files
        // nec files will just be overwritten and the wall clock time isn't bad even for exhaustive!
        bool m_run_simulator;
        float m_exp_weight;
        float m_mutation;
        float m_max_gain_fitness;
        float m_max_coup_fitness;
        float m_min_coup_fitness;
        std::string m_lua_file;
        std::vector<individual_ptr> m_free_inds;

        //ant_config_ptr clone(ant_config_ptr);
        unsigned int m_min_theta = 0;
        float m_step_theta = 46;
        float m_incr_theta = 4;
        float m_min_phi = 0;
        float m_step_phi = 90;
        float m_incr_phi = 4;
        float m_min_freq = 100;
        unsigned int m_step_freq = 1;
        float m_incr_freq = 10;

        const float gain_wt = 0.5;
        const float coupling_wt = 0.5;

        unsigned int num_polar(void);
        void write_platform(std::ofstream&);
        void write_ant(std::ofstream&, ant_config_ptr&, position_ptr&, unsigned int);
        void write_excitation(std::ofstream&, unsigned int);
        void write_coupling(std::ofstream&, std::vector<int>&);
        individual_ptr create_individual(std::string, std::vector<position_ptr> &);
        float compare(const evaluation_ptr &, const evaluation_ptr &);
        std::vector<individual_ptr> breed(const individual_ptr&, const individual_ptr&);
        void simple_mutation(individual_ptr&);
        bool overlap(std::vector<position_ptr>&, position_ptr&);
        void save_best_nec(const std::string&, individual_ptr&);
        void save_population(const std::string&, std::vector<individual_ptr>&);
        void save_norm(const std::string&);
        unsigned int read_radiation(const std::string, const evaluation_ptr &);
        float read_coupling(const std::string, unsigned int);
        float cal_fitness(individual_ptr&);

    public:
        algorithm(std::string);
        virtual ~algorithm(void);

        std::vector<ant_config_ptr> m_ant_configs; //stores antennas positions and all wires mentioned in the nec file
        ant_config_ptr m_platform;

        void setup_ant_placements();
        void load_nec_files();
        void setup_run_context();
        void write_freespace();
        void run_freespace();
        void read_freespace();

        virtual void setup_algo_params();
        virtual void run() = 0;
        virtual void run_simulation(unsigned int) = 0;
};

// body of an inline function needs to be in the header so that the compiler can actually replace it 
inline float algorithm::cal_fitness(individual_ptr &ind)
{
    return ((gain_wt * ind->m_gain_fitness) + (coupling_wt * ind->m_coupling_fitness));
}

inline unsigned int algorithm::num_polar(void)
{
    return m_step_theta * m_step_phi;
}

#endif
