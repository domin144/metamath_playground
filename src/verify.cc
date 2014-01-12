#include <set>
#include <algorithm>
#include <cassert>
#include <stack>

#include "Statement_visitors.h"
#include "verify.h"

class Frame
{
public:
    Frame( const Assertion *assertion )
    {
        m_statements.push_back( assertion );
    }
    std::vector<const Statement *> &get_statements()
    {
        return m_statements;
    }
    const std::vector<const Statement *> &get_statements() const
    {
        return m_statements;
    }

private:
    std::vector<const Statement *> m_statements;
};
//------------------------------------------------------------------------------
class Extended_frame_collector : public Lazy_const_statement_visitor
{
public:
    Extended_frame_collector( Frame &extended_frame ) :
        m_extended_frame( extended_frame )
    { }

    void operator()( const Essential_hypothesis *hypothesis ) override
    {
        collect_statement( hypothesis );
    }
    void operator()( const Floating_hypothesis *hypothesis ) override
    {
        collect_statement( hypothesis );
    }
    void operator()( const Disjoint_variable_restriction *restriction ) override
    {
        collect_statement( restriction );
    }

private:
    void collect_statement( const Statement *statment )
    {
        m_extended_frame.get_statements().push_back( statment );
    }
    Frame &m_extended_frame;
};
//------------------------------------------------------------------------------
void collect_extended_frame( Frame &frame )
{
    Extended_frame_collector collector( frame );
    auto statement = frame.get_statements().front();
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
    const Expression_holder *holder )
{
    Variable_collector collector( variables );
    for( auto &symbol : holder->get_expression() )
    {
        symbol.accept( collector );
    }
}
//------------------------------------------------------------------------------
void extract_variables( std::vector<const Variable *> &variables, Frame &frame )
{
    auto assertion = statement_cast<const Assertion *>(
        frame.get_statements().front() );
    assert( assertion );
    extract_variables( variables, assertion );
    for( auto statement : frame.get_statements() )
    {
        auto hypothesis =
            statement_cast<const Essential_hypothesis *>( statement );
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

    typedef const Statement *argument_type;
    bool operator()( const Statement *statement ) const
    {
        class Visitor : public Lazy_const_statement_visitor
        {
        public:
            Visitor( const std::vector<const Variable *> &variables ) :
                m_variables( variables )
            { }
            void operator()( const Floating_hypothesis *hypothesis ) override
            {
                auto &variable = hypothesis->get_expression().front();
                m_result = std::find_if( m_variables.begin(), m_variables.end(),
                    [&variable]( const Variable *x ) { return x==&variable; } )
                        == m_variables.end();
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
typedef std::map< const Symbol *, Expression> Substitution_map;
//------------------------------------------------------------------------------
class Symbol_substitutor : public Const_symbol_visitor
{
public:
    void operator()( const Constant *constant ) override
    {
        /* TODO: copy constant */
    }
    void operator()( const Variable *variable ) override
    {
        /* TODO: substitute variable */
    }
};
//------------------------------------------------------------------------------
void apply_substitution( const Substitution_map &substitution,
    const Expression &source, Expression &target )
{
    /* TODO: apply Symbol_substitutor */
}
//------------------------------------------------------------------------------
class Substitution_collector : private Lazy_const_statement_visitor
{
private:
    void operator()( const Essential_hypothesis *hypothesis ) override
    {
        /* TODO: verify substitution */
    }
    void operator()( const Floating_hypothesis *hypothesis ) override
    {
        /* TODO: put substitution into map*/
    }
};
//------------------------------------------------------------------------------
void fill_substitution_map( Substitution_map &map,
    const std::vector<Expression> &source,
    const std::vector<const Statement *> &target )
{
    /* TODO: collect and verify substitution */
}
//------------------------------------------------------------------------------
class Proof_stack : private Const_statement_visitor
{
public:
    Proof_stack( const Frame &frame ) :
        m_frame( frame ),
        m_result( true )
    { }
    const Expression &get_top() const
    {
        return m_stack.back();
    }
    bool push( const Statement *statement )
    {
        statement->accept( *this );
        return m_result;
    }

private:
    void operator()( const Scoping_statement * ) override
    {
        invalid_statement();
    }
    void operator()( const Constant_declaration * ) override
    {
        invalid_statement();
    }
    void operator()( const Variable_declaration * ) override
    {
        invalid_statement();
    }
    void operator()( const Axiom *axiom ) override
    {
        push_assertion( axiom );
    }
    void operator()( const Theorem *theorem ) override
    {
        push_assertion( theorem );
    }
    void operator()( const Essential_hypothesis *hypothesis ) override
    {
        push_hypothesis( hypothesis, hypothesis->get_expression() );
    }
    void operator()( const Floating_hypothesis *hypothesis ) override
    {
        push_hypothesis( hypothesis, hypothesis->get_expression() );
    }
    void operator()( const Disjoint_variable_restriction * )
    {
        invalid_statement();
    }
    void invalid_statement()
    {
        throw( std::runtime_error( "invalid statement found in proof" ) );
    }
    void push_assertion( const Assertion *assertion )
    {
        Frame frame( assertion );
        collect_frame( frame );

        Substitution_map substitution_map;
        fill_substitution_map( substitution_map, m_stack,
            frame.get_statements() );
        /* TODO */
    }
    void push_hypothesis( const Statement *statement,
        const Expression &expression )
    {
        auto available_statements = m_frame.get_statements();
        const bool available = std::find_if( available_statements.begin(),
            available_statements.end(),
            [statement]( const Statement *x ) { return x==statement; } ) ==
            available_statements.end();
        if( available )
        {
            m_stack.push_back( expression );
        }
        m_result = available;
    }

private:
    const Frame &m_frame;
    std::vector<Expression> m_stack;
    bool m_result;
};
//------------------------------------------------------------------------------
bool verify( const Theorem *theorem )
{
    Frame frame( theorem );
    collect_frame( frame );
    Proof_stack stack( frame );
    bool result = true;

    for( auto statement : theorem->get_proof() )
    {
        result = stack.push( statement );
        if( !result )
        {
            break;
        }
    }
    /* TODO: check if expression on top if the stack is the expression of the
       therorem being proven */
    return result;
}
//------------------------------------------------------------------------------
bool verify( const Metamath_database &db )
{
    return false;
}
