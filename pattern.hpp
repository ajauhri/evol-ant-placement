#ifndef PATTERN_HPP_INCLUDED
#define PATTERN_HPP_INCLUDED
#include<boost/shared_ptr.hpp>
#include<vector>
class pattern
{
    public:
        pattern(void);

        std::vector<float> m_db_gain;
        std::vector<float> m_quad_db;
        float m_frequency;
};
typedef boost::shared_ptr<pattern> pattern_ptr;
#endif
