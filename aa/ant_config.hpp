#ifndef ANT_CONFIG_HPP_INCLUDED
#define ANT_CONFIG_HPP_INCLUDED
#include "position.hpp"
#include<string>
#include<vector>
#include<boost/shared_ptr.hpp>
class ant_config
{
public:
	std::string name;
	std::vector<position_ptr> positions;
};
typedef boost::shared_ptr<ant_config> ant_config_ptr;
#endif
