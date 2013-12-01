#include <stdexcept>
#include "Metamath_database.h"

Metamath_database::Metamath_database()
{
    auto invalid_axiom = new Axiom( "?" );
    m_top_scope.add_statement( invalid_axiom );
}
//------------------------------------------------------------------------------
Scoping_statement *Metamath_database::get_top_scope()
{
    return &m_top_scope;
}
