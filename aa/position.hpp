#if 0
#ifndef POSITION_HPP_INCLUDED
#define POSITION_HPP_INCLUDED
#include<string>
#include<boost/shared_ptr.hpp>
class position
{
public:
	float x;
	float y;
	float z;
};
typedef boost::shared_ptr<position> position_ptr;
#endif
#endif
