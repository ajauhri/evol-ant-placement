#ifndef ES_HPP_INCLUDED
#define ES_HPP_INCLUDED
#include<algorithm.hpp>

class es :
	public algorithm
{
	private: 
		unsigned int m_mu ;
		unsigned int m_lambda;
		unsigned int m_generations;
        unsigned int m_mulambda_factor;
        std::vector<individual_ptr> m_pop;


        void run_simulation(unsigned int);
        void survivor_selection(void);
        void create_pop(unsigned int);
        void evaluate_gen(unsigned int);
	public:
		es(std::string);
		~es(void);

		void setup_algo_params();
		void run();
};
#endif
