#include "Scope.h"

Scoping_statement::Scoping_statement( Scoping_statement *parrent ) :
    m_parrent( parrent )
{ }
//------------------------------------------------------------------------------
Scoping_statement::~Scoping_statement()
{
    Statement *statement = m_first;
    while( statement )
    {
        Statement *old = statement;
        statement = statement->get_next();
        delete old;
    }
}
//------------------------------------------------------------------------------
bool Scoping_statement::is_top() const
{
    return !m_parrent;
}
//------------------------------------------------------------------------------
Symbol *Scoping_statement::get_symbol_by_label( const std::string &label)
{
    auto i = m_label_to_symbol.find( label );
    if( i!=m_label_to_symbol.end() )
    {
        return i->second;
    }
    else if( !is_top() )
    {
        return m_parrent->get_symbol_by_label( label );
    }
    else
    {
        throw( std::runtime_error( "symbol \"" + label + "\" not found" ) );
    }
}
//------------------------------------------------------------------------------
Named_statement *Scoping_statement::get_statement_by_label(
    const std::string &label )
{
    auto i = m_label_to_statement.find( label );
    if( i!=m_label_to_statement.end() )
    {
        return i->second;
    }
    else if( !is_top() )
    {
        return m_parrent->get_statement_by_label( label );
    }
    else
    {
        throw( std::runtime_error( "statement \"" + label + "\" not found" ) );
    }
}
//------------------------------------------------------------------------------
namespace
{
Scoping_statement *get_top( Scoping_statement *scope )
{
    while( !scope->is_top() )
        scope = scope->get_parrent();
    return scope;
}
}
//------------------------------------------------------------------------------
void Scoping_statement::add_statement( Statement *statement )
{
    if( m_first && m_last )
    {
        statement->set_previous( m_last );
        m_last->set_next( statement );
        m_last = statement;
    }
    else
    {
        m_first = m_last = statement;
    }
    switch( statement->get_type() )
    {
    case Statement::Type::axiom:
    case Statement::Type::theorem:
        {
            auto named_statement = static_cast<Named_statement *>( statement );
            get_top( this )->m_label_to_statement[named_statement->get_name()] =
                named_statement;
        }
        break;
    case Statement::Type::constant_declaration:
    case Statement::Type::variable_declaration:
        for( auto symbol : statement->get_expression() )
        {
            m_label_to_symbol[symbol->get_name()] = symbol;
        }
        break;
    case Statement::Type::floating_hypothesis:
    case Statement::Type::essential_hypothesis:
        {
            auto named_statement = static_cast<Named_statement *>( statement );
            m_label_to_statement[named_statement->get_name()] =
                named_statement;
        }
        break;
    default:
        ;
    }
}
//------------------------------------------------------------------------------
Scoping_statement *Scoping_statement::get_parrent()
{
    return m_parrent;
}
//------------------------------------------------------------------------------
Statement *Scoping_statement::get_first()
{
    return m_first;
}
