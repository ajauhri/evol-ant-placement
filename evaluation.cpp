#include<evaluation.hpp>

evaluation::evaluation() :
    max_db(0.0f),
    min_db(0.0f)
{ }

evaluation::~evaluation(void)
{
    radiation.clear();
    radiation.shrink_to_fit();
    std::cout<<"evaluation destructor called\n";

}

