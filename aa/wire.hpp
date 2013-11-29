#ifndef WIRE_HPP_INCLUDED
#define WIRE_HPP_INCLUDED
#include<position.hpp>
#include<boost/shared_ptr.hpp>
class wire
{	
    public:
        position a;
        position b;
        int segments;
        float diameter;
};
typedef boost::shared_ptr<wire> wire_ptr;
#endif

