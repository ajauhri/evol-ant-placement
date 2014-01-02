#ifndef EXHAUST_HPP_INCLUDED
#define EXHAUST_HPP_INCLUDED
#include<algorithm.hpp>
#include<individual.hpp>
#include<vector>
#include<boost/format.hpp>
class exhaust : public algorithm
{
    private:
        std::vector<individual_ptr> m_pop;
        boost::format m_nec_input;

        void run_simulation(unsigned int);
        void evaluate();
        void recur_placements(std::vector<position_ptr>&, unsigned int);

    public:
        exhaust(std::string);
        ~exhaust(void);

        void setup_algo_params();
        void run(unsigned int);
};
#endif

