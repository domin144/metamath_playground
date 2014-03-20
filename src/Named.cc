#include "Named.h"

Named::Named( const std::string &name ) :
    m_name( name )
{ }
//------------------------------------------------------------------------------
const std::string &Named::get_name() const
{
    return m_name;
}
//------------------------------------------------------------------------------
bool Less_by_name::operator()( const Named &lhs, const Named &rhs ) const
{
    return lhs.get_name() < rhs.get_name();
}
