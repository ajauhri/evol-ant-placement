#if 0
#ifndef TARGET_HPP_INCLUDED
#define TARGET_HPP_INCLUDED
#include<string>
#include<vector>
#include<boost/shared_ptr.hpp>
class target
{
public:
	float freq;
	std::string filename;
	std::vector<float> db_gain;
};
typedef boost::shared_ptr<target> target_ptr;
#endif
#endif
