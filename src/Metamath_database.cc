#include <stdexcept>
#include "Metamath_database.h"

Metamath_database::Metamath_database()
{ }
//------------------------------------------------------------------------------
Scoping_statement *Metamath_database::get_top_scope()
{
    return &m_top_scope;
}
//------------------------------------------------------------------------------
const Scoping_statement *Metamath_database::get_top_scope() const
{
    return &m_top_scope;
}
