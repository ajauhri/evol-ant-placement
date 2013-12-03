#ifndef PATTERN_HPP_INCLUDED
#define PATTERN_HPP_INCLUDED
#include<boost/shared_ptr.hpp>
#include<vector>
class pattern
{
    float *db_gain;
    float *quad_db;
    float frequency;
    unsigned int db_count;
    unsigned int id;
};
typedef boost::shared_ptr<pattern> pattern_ptr;
#endif

