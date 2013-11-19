#if 0
#ifndef ANCILLARY_CONFIG_HPP_INCLUDED
#define ANCILLARY_CONFIG_HPP_INCLUDED
#include "position.hpp"
#include<string>
#include<vector>
#include<boost/shared_ptr.hpp>

class ancillary_config
{
public:
	std::string name;
	std::string ancilary_locator;
	std::string mount_object;
	std::string mount_object_locator;
	std::string translation;
	std::string rotation;
};
typedef boost::shared_ptr<ancillary_config> ancillary_config_ptr;
#endif
#endif
