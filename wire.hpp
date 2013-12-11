#ifndef WIRE_HPP_INCLUDED
#define WIRE_HPP_INCLUDED
#include<position.hpp>
#include<boost/shared_ptr.hpp>
class wire
{	
    public:
        wire(void);
        position_ptr mp_a;
        position_ptr mp_b;
        int m_segments;
        float m_diameter;
};
typedef boost::shared_ptr<wire> wire_ptr;
#endif

