#ifndef EVALUATION_HPP_INCLUDED
#define EVALUATION_HPP_INCLUDED
#include<boost/shared_ptr.hpp>
#include<vector>
#include<pattern.hpp>
class evaluation
{
    public:
        evaluation(void);

        std::vector<pattern_ptr> m_radiation;
        float m_max_db;
        float m_min_db;
};
typedef boost::shared_ptr<evaluation> evaluation_ptr;
#endif
