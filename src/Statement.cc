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
const Statement *Statement::get_previous() const
{
    return m_previous;
}
//------------------------------------------------------------------------------
const Statement *Statement::get_next() const
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
Expression &Expression_holder::get_expression()
{
    return m_expression;
}
//------------------------------------------------------------------------------
const Expression &Expression_holder::get_expression() const
{
    return m_expression;
}
//------------------------------------------------------------------------------
Constant_declaration::Constant_declaration()
{ }
//------------------------------------------------------------------------------
Constant_declaration::~Constant_declaration()
{
    while( !get_expression().empty() )
    {
        delete get_expression().back();
        get_expression().pop_back();
    }
}
//------------------------------------------------------------------------------
Variable_declaration::Variable_declaration()
{ }
//------------------------------------------------------------------------------
Variable_declaration::~Variable_declaration()
{
    while( !get_expression().empty() )
    {
        delete get_expression().back();
        get_expression().pop_back();
    }
}
//------------------------------------------------------------------------------
Axiom::Axiom( const std::string &label ) :
    Assertion( label )
{ }
//------------------------------------------------------------------------------
Theorem::Theorem( const std::string &label ) :
    Assertion( label )
{ }
//------------------------------------------------------------------------------
Proof &Theorem::get_proof()
{
    return m_proof;
}
//------------------------------------------------------------------------------
const Proof &Theorem::get_proof() const
{
    return m_proof;
}
//------------------------------------------------------------------------------
Essential_hypothesis::Essential_hypothesis( const std::string &label ) :
    Named_statement( label )
{ }
//------------------------------------------------------------------------------
Floating_hypothesis::Floating_hypothesis( const std::string &label ) :
    Named_statement( label )
{ }
//------------------------------------------------------------------------------
Disjoint_variable_restriction::Disjoint_variable_restriction()
{ }
