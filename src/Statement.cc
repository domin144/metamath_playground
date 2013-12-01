#include "Statement.h"

Statement::Statement()
{ }
//------------------------------------------------------------------------------
Statement::~Statement()
{ }
//------------------------------------------------------------------------------
Statement *Statement::get_previous()
{
    return m_previous;
}
//------------------------------------------------------------------------------
Statement *Statement::get_next()
{
    return m_next;
}
//------------------------------------------------------------------------------
void Statement::set_previous( Statement *previous )
{
    m_previous = previous;
}
//------------------------------------------------------------------------------
void Statement::set_next( Statement *next )
{
    m_next = next;
}
//------------------------------------------------------------------------------
std::vector<Symbol *> &Statement::get_expression()
{
    return m_expression;
}
//------------------------------------------------------------------------------
Constant_declaration::Constant_declaration()
{ }
//------------------------------------------------------------------------------
Constant_declaration::~Constant_declaration()
{
    for( auto constant : get_expression() )
        delete constant;
}
//------------------------------------------------------------------------------
Variable_declaration::Variable_declaration()
{ }
//------------------------------------------------------------------------------
Variable_declaration::~Variable_declaration()
{
    for( auto variable : get_expression() )
        delete variable;
}
//------------------------------------------------------------------------------
Axiom::Axiom( const std::string &label ) :
    Typed_statement( label )
{ }
//------------------------------------------------------------------------------
Theorem::Theorem( const std::string &label ) :
    Typed_statement( label )
{ }
//------------------------------------------------------------------------------
std::vector<Named_statement *> &Theorem::get_proof()
{
    return m_proof;
}
//------------------------------------------------------------------------------
Essential_hypothesis::Essential_hypothesis( const std::string &label ) :
    Typed_statement( label )
{ }
//------------------------------------------------------------------------------
Floating_hypothesis::Floating_hypothesis( const std::string &label ) :
    Typed_statement( label )
{ }
//------------------------------------------------------------------------------
Disjoint_variable_restriction::Disjoint_variable_restriction()
{ }
