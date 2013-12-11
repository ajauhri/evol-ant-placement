#include<position.hpp>
// does not have an implicit typecast which i.e. initializing with 0.0f and implicit typecast can require the compiler to generate extra code

position::position() :
    m_x(0.0f),
    m_y(0.0f),
    m_z(0.0f)
{}
