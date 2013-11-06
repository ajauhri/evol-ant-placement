#ifndef POSITION_HPP_INCLUDED
#define POSITION_HPP_INCLUDED
#include<string>
#include<boost/shared_ptr.hpp>
class position
{
public:
	std::string mount_object;
	std::string mount_object_locator;
	std::string translation;
	std::string rotation;
};
typedef boost::shared_ptr<position> position_ptr;
#endif
