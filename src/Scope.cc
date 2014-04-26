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
const Symbol *Scoping_statement::get_symbol_by_label( const std::string &label )
    const
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
const Named_statement *Scoping_statement::get_statement_by_label(
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
        statement->set_previous( this );
    }

    class Name_registrator : public Statement_visitor
    {
    public:
        Name_registrator( Scoping_statement &scope ) :
            m_scope( scope )
        { }
        void operator()( Scoping_statement * ) override
        { }
        void operator()( Constant_declaration *declaration ) override
        { }
        void operator()( Variable_declaration *declaration ) override
        { }
        void operator()( Axiom *axiom ) override
        {
            get_top( &m_scope )->m_label_to_statement[axiom->get_name()] =
                axiom;
        }
        void operator()( Theorem *theorem ) override
        {
            get_top( &m_scope )->m_label_to_statement[theorem->get_name()] =
                theorem;
        }
        void operator()( Essential_hypothesis *hypothesis ) override
        {
            m_scope.m_label_to_statement[hypothesis->get_name()] = hypothesis;
        }
        void operator()( Floating_hypothesis *hypothesis ) override
        {
            m_scope.m_label_to_statement[hypothesis->get_name()] = hypothesis;
        }
        void operator()( Disjoint_variable_restriction * ) override
        { }
    private:
        Scoping_statement &m_scope;
    };

    Name_registrator registrator( *this );
    statement->accept( registrator );
}
//------------------------------------------------------------------------------
void Scoping_statement::register_symbol( const Symbol *symbol )
{
    try
    {
        get_symbol_by_label( symbol->get_name() );
        throw std::runtime_error( "symbol already present when trying to "
            "register");
    }
    catch( std::runtime_error & )
    {
        m_label_to_symbol[symbol->get_name()] = symbol;
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
//------------------------------------------------------------------------------
const Statement *Scoping_statement::get_first() const
{
    return m_first;
}
