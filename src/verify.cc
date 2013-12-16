#include <set>
#include <algorithm>
#include <cassert>

#include "Statement_visitors.h"
#include "verify.h"

class Frame
{
public:
    Frame( Assertion *assertion )
    {
        m_statements.push_back( assertion );
    }
    std::vector<Statement *> &get_statements()
    {
        return m_statements;
    }

private:
    std::vector<Statement *> m_statements;
};
//------------------------------------------------------------------------------
class Extended_frame_collector : public Lazy_statement_visitor
{
public:
    Extended_frame_collector( Frame &extended_frame ) :
        m_extended_frame( extended_frame )
    { }

    void operator()( Essential_hypothesis *hypothesis ) override
    {
        collect_statement( hypothesis );
    }
    void operator()( Floating_hypothesis *hypothesis ) override
    {
        collect_statement( hypothesis );
    }
    void operator()( Disjoint_variable_restriction *restriction ) override
    {
        collect_statement( restriction );
    }

private:
    void collect_statement( Statement *statment )
    {
        m_extended_frame.get_statements().push_back( statment );
    }
    Frame &m_extended_frame;
};
//------------------------------------------------------------------------------
void collect_extended_frame( Frame &frame )
{
    Extended_frame_collector collector( frame );
    Statement *statement = frame.get_statements().front();
    while( statement )
    {
        statement->accept( collector );
        statement = statement->get_previous();
    }
}
//------------------------------------------------------------------------------
class Variable_collector : public Const_symbol_visitor
{
public:
    Variable_collector( std::vector<const Variable *> variables ) :
        m_variables( variables )
    { }
    void operator()( const Variable *variable ) override
    {
        m_variables.push_back( variable );
    }
    void operator()( const Constant * ) override
    { }
private:
    std::vector<const Variable *> m_variables;
};
//------------------------------------------------------------------------------
void extract_variables( std::vector<const Variable *> &variables,
    Expression_holder *holder )
{
    Variable_collector collector( variables );
    for( auto symbol : holder->get_expression() )
    {
        symbol->accept( collector );
    }
}
//------------------------------------------------------------------------------
void extract_variables( std::vector<const Variable *> &variables, Frame &frame )
{
    auto assertion = statement_cast<Assertion *>(
        frame.get_statements().front() );
    assert( assertion );
    extract_variables( variables, assertion );
    for( auto statement : frame.get_statements() )
    {
        auto hypothesis = statement_cast<Essential_hypothesis *>( statement );
        if( hypothesis )
        {
            extract_variables( variables, hypothesis );
        }
    }
}
//------------------------------------------------------------------------------
class IsMandatory
{
public:
    IsMandatory( const std::vector<const Variable *> &variables ) :
        m_variables( variables )
    { }

    typedef Statement *argument_type;
    bool operator()( const Statement *statement ) const
    {
        class Visitor : public Lazy_const_statement_visitor
        {
        public:
            Visitor( const std::vector<const Variable *> &variables ) :
                m_variables( variables )
            { }
            void operator()( const Floating_hypothesis *hypothesis )
            {
                auto variable = hypothesis->get_expression().front();
                m_result = std::find( m_variables.begin(), m_variables.end(),
                    variable ) == m_variables.end();
            }
            bool operator()()
            {
                return m_result;
            }

        private:
            const std::vector<const Variable *> &m_variables;
            bool m_result;
        } visitor( m_variables );

        statement->accept( visitor );
        return visitor();
    }

private:
    const std::vector<const Variable *> &m_variables;
};
//------------------------------------------------------------------------------
void collect_frame( Frame &frame )
{
    collect_extended_frame( frame );

    std::vector<const Variable *> variables;
    extract_variables( variables, frame );

    auto &statements = frame.get_statements();
    statements.erase(
        std::remove_if( statements.begin(), statements.end(),
            std::not1( IsMandatory( variables ) ) ),
        statements.end() );
}
//------------------------------------------------------------------------------
bool verify( Metamath_database &db )
{
    Axiom x("x");
    Frame frame( &x );
    collect_frame( frame );
    return false;
}
