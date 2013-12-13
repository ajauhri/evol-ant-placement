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
        float m_exp_weight;
        float m_mutation;
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

    public:
        algorithm(std::string);
        virtual ~algorithm(void);

        std::vector<ant_config_ptr> m_ant_configs; //stores antennas positions and all wires mentioned in the nec file
        ant_config_ptr m_platform;

        virtual void setup_run_context();
        virtual void setup_algo_params();
        virtual void setup_ant_placements();
        virtual void write_freespace();
        virtual void load_nec_files();
        virtual void run_freespace();
        virtual void read_freespace();
        virtual unsigned int read_nou(const std::string, const evaluation_ptr &);
        virtual void run() = 0;
        virtual void run_simulation(unsigned int) = 0;
};

#endif
