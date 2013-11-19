#if 0
#ifndef INDIVIDUAL_HPP_INCLUDED
#define INDIVIDUAL_HPP_INCLUDED
#include "ant_config.hpp"
#include<string>
#include<vector>
#include<boost/shared_ptr.hpp>
class individual {
public:
	float fitness; /*average over all runs*/
	std::vector<ant_config_ptr> ant_configs;
	individual() { fitness=0.0f;} 
};
typedef boost::shared_ptr<individual> individual_ptr;
#endif
#endif
