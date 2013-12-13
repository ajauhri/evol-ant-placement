#ifndef ES_HPP_INCLUDED
#define ES_HPP_INCLUDED
#include<algorithm.hpp>

class es :
	public algorithm
{
	private: 
		unsigned int mu ;
		unsigned int lambda;
		unsigned int generations;

        void run_simulation(unsigned int);
	public:
		es(std::string);
		~es(void);

		void setup_algo_params();
		void run();
        void save_best_nec();
};
#endif
