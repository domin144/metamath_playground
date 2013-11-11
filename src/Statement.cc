#include "Statement.h"

Statement::Statement( const std::string &name ) :
    Named( name )
{
}
//------------------------------------------------------------------------------
void Statement::push_back( const Symbol &symbol )
{
    m_content.push_back( &symbol );
}
//------------------------------------------------------------------------------
Axiom::Axiom( const std::string &name ) :
    Statement( name )
{ }
//------------------------------------------------------------------------------
Theorem::Theorem( const std::string &name ) :
    Statement( name )
{ }
//------------------------------------------------------------------------------
Assumption::Assumption( const std::string &name ) :
    Statement( name )
{ }
//------------------------------------------------------------------------------
Variable_assumption::Variable_assumption( const std::string &name ) :
    Statement( name )
{ }
