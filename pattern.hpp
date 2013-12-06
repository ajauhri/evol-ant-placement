#ifndef PATTERN_HPP_INCLUDED
#define PATTERN_HPP_INCLUDED
#include<boost/shared_ptr.hpp>
#include<vector>
class pattern
{
    public:
        pattern(void)
        { }
        ~pattern(void)
        {
            //need to cout here and check if theser are deallocated at the end of each generation
            db_gain.clear();
            db_gain.shrink_to_fit();

            quad_db.clear();
            quad_db.shrink_to_fit();
        }

        std::vector<float> db_gain;
        std::vector<float> quad_db;
        float frequency;
        unsigned int id;
};
typedef boost::shared_ptr<pattern> pattern_ptr;
#endif
