#ifndef INDIVIDUAL_HPP_INCLUDED
#define INDIVIDUAL_HPP_INCLUDED
#include<ant_config.hpp>
#include<evaluation.hpp>
#include<string>
#include<vector>
#include<boost/shared_ptr.hpp>
class individual {
public:
	float m_fitness;
    std::vector<float> m_one_ant_on_fitness;
    std::vector<position_ptr> m_positions;
    std::vector<evaluation_ptr> m_evals;

	individual(void);
};
typedef boost::shared_ptr<individual> individual_ptr;
#endif
