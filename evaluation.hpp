#ifndef EVALUATION_HPP_INCLUDED
#define EVALUATION_HPP_INCLUDED
#include<boost/shared_ptr.hpp>
#include<vector>
#include<pattern.hpp>
class evaluation
{
    public:
        evaluation(void);
        ~evaluation(void);

        std::vector<pattern_ptr> radiation;
        float max_db;
        float min_db;
};
typedef boost::shared_ptr<evaluation> evaluation_ptr;
#endif
