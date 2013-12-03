#ifndef EVAUATION_HPP_INCLUDED
#define EVALUATION_HPP_INCLUDED
#include<boost/shared_ptr.hpp>
#include<vector>
#include<radiation.hpp>
class evaluation
{
    public:
        evaluation(void);
        ~evaluation(void)
        {
            radiation.clear();
            radiation.shrink_to_fit();
        }

        std::vector<pattern_ptr> radiation;
        float max_db;
        float min_db;
};
#endif
