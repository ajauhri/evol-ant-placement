#include<individual.hpp>
individual::individual() :
    fitness(0.0f)
{}

individual::~individual()
{
    ant_configs.clear();
    ant_configs.shrink_to_fit();
}

