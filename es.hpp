#ifndef ES_HPP_INCLUDED
#define ES_HPP_INCLUDED
#include "algorithm.hpp"

class es :
	public algorithm
{
	private: 
		typedef algorithm super;
		unsigned int mu ;
		unsigned int lambda;
		unsigned int generations;

	public:
		es(std::string);
		~es(void);

		void setup_algo_params();
		void run();
        void run_simulation(unsigned int);
};
#endif
