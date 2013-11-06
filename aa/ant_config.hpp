#ifndef ANT_CONFIG_HPP_INCLUDED
#define ANT_CONFIG_HPP_INCLUDED
#include "position.hpp"
#include "target.hpp"
#include<string>
#include<vector>
#include<boost/shared_ptr.hpp>
class ant_config
{
public:
	std::string name;
	bool pattern_available;
	std::string antenna_locator; //#TODO is the data or metadata. Need to be precise with the XML definition of it being a element or attribute
	std::vector<position_ptr> positions;
	std::vector<target_ptr> targets;
};
typedef boost::shared_ptr<ant_config> ant_config_ptr;
#endif
