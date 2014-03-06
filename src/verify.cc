#include <set>
#include <algorithm>
#include <cassert>
#include <stack>

#include "Statement_visitors.h"
#include "Symbol_visitors.h"
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
    std::vector<const Disjoint_variable_restriction *> &get_restrictions()
    {
        return m_restrictions;
    }
    const std::vector<const Disjoint_variable_restriction *> &get_restrictions()
        const
    {
        return m_restrictions;
    }

private:
    std::vector<const Statement *> m_statements;
    std::vector<const Disjoint_variable_restriction *> m_restrictions;
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
        collect_hypothesis( hypothesis );
    }
    void operator()( const Floating_hypothesis *hypothesis ) override
    {
        collect_hypothesis( hypothesis );
    }
    void operator()( const Disjoint_variable_restriction *restriction ) override
    {
        collect_restriction( restriction );
    }

private:
    void collect_hypothesis( const Statement *statment )
    {
        m_extended_frame.get_statements().push_back( statment );
    }
    void collect_restriction( const Disjoint_variable_restriction *restriction )
    {
        m_extended_frame.get_restrictions().push_back( restriction );
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
    std::reverse( frame.get_statements().begin(),
        frame.get_statements().end() );
}
//------------------------------------------------------------------------------
class Variable_collector : public Const_symbol_visitor
{
public:
    Variable_collector( std::vector<const Variable *> &variables ) :
        m_variables( variables )
    { }
    void operator()( const Variable *variable ) override
    {
        const bool variable_present = std::find( m_variables.begin(),
            m_variables.end(), variable ) != m_variables.end();
        if( !variable_present )
            m_variables.push_back( variable );
    }
    void operator()( const Constant * ) override
    { }
private:
    std::vector<const Variable *> &m_variables;
};
//------------------------------------------------------------------------------
void extract_variables( std::vector<const Variable *> &variables,
    const Expression_holder *holder )
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
    auto assertion = statement_cast<const Assertion *>(
        frame.get_statements().back() );
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
                m_variables( variables ),
                m_result( true )
            { }
            void operator()( const Floating_hypothesis *hypothesis ) override
            {
                auto variable = hypothesis->get_expression().back();
                assert( symbol_cast<const Variable *>( variable ) );
                m_result = std::find( m_variables.begin(), m_variables.end(),
                    variable ) != m_variables.end();
            }
            void operator()( const Disjoint_variable_restriction *restriction )
                override
            {
                auto variable_0 = restriction->get_expression().at(0);
                auto variable_1 = restriction->get_expression().at(1);
                const bool present_0 =
                    std::find( m_variables.begin(), m_variables.end(),
                    variable_0 ) != m_variables.end();
                const bool present_1 =
                    std::find( m_variables.begin(), m_variables.end(),
                    variable_1 ) != m_variables.end();
                m_result = present_0 && present_1;
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

    auto &restrictions = frame.get_restrictions();
    restrictions.erase(
        std::remove_if( restrictions.begin(), restrictions.end(),
            std::not1( IsMandatory( variables ) ) ),
        restrictions.end() );
}
//------------------------------------------------------------------------------
typedef std::map< const Symbol *, Expression> Substitution_map;
//------------------------------------------------------------------------------
class Symbol_substitutor : private Const_symbol_visitor
{
public:
    Symbol_substitutor( const Substitution_map &substitution,
        const Expression &base, Expression &effect
    ) :
        m_substitution( substitution ),
        m_base( base ),
        m_effect( effect )
    { }
    void substitute()
    {
        for( auto symbol : m_base )
        {
            symbol->accept( *this );
        }
    }

private:
    void operator()( const Constant *constant ) override
    {
        // copy constant
        m_effect.push_back( constant );
    }
    void operator()( const Variable *variable ) override
    {
        // substitute variable
        auto &substituted_expression = m_substitution.at( variable );
        m_effect.insert( m_effect.end(), substituted_expression.begin(),
            substituted_expression.end() );
    }
    const Substitution_map &m_substitution;
    const Expression &m_base;
    Expression &m_effect;
};
//------------------------------------------------------------------------------
void apply_substitution( const Substitution_map &substitution,
    const Expression &base, Expression &effect )
{
    Symbol_substitutor symbol_substitutor( substitution, base, effect );
    symbol_substitutor.substitute();
}
//------------------------------------------------------------------------------
class Substitution_collector : private Lazy_const_statement_visitor
{
public:
    Substitution_collector( Substitution_map &map,
        const std::vector<Expression> &effect,
        const std::vector<const Statement *> &base
    ) :
        m_map( map ),
        m_effect( effect ),
        m_base( base ),
        m_effect_iterator( effect.begin() ),
        m_result( true )
    {
        if( base.size() != effect.size() )
        {
            throw( std::runtime_error( "count of base expression does not "
                "match effect statesments count" ) );
        }
    }
    bool collect()
    {
        for( auto statement : m_base )
        {
            statement->accept( *this );
            ++m_effect_iterator;
            if( !m_result )
                return false;
        }
        return m_result;
    }

private:
    void operator()( const Essential_hypothesis *hypothesis ) override
    {
        auto &effect_expression = *m_effect_iterator;
        auto &base_expression = hypothesis->get_expression();

        // verify substitution
        Expression substitution_result;
        apply_substitution( m_map, base_expression, substitution_result );

        if( substitution_result != effect_expression )
        {
            m_result = false;
            return;
        }
    }
    void operator()( const Floating_hypothesis *hypothesis ) override
    {
        auto &effect_expression = *m_effect_iterator;
        auto &base_expression = hypothesis->get_expression();

        // add substitution for variable
        if( base_expression[0] != effect_expression[0] )
        {
            m_result = false;
            return;
        }

        if( m_map.find( base_expression[1] ) != m_map.end() )
        {
            m_result = false;
            return;
        }

        m_map[base_expression[1]].assign(
            ++effect_expression.begin(), effect_expression.end() );
    }

    Substitution_map &m_map;
    const std::vector<Expression> &m_effect;
    const std::vector<const Statement *> &m_base;
    std::vector<Expression>::const_iterator m_effect_iterator;
    bool m_result;
};
//------------------------------------------------------------------------------
bool fill_substitution_map( Substitution_map &map,
    const std::vector<Expression> &effect,
    const std::vector<const Statement *> &base )
{
    Substitution_collector substitution_collector( map, effect, base );
    return substitution_collector.collect();
}
//------------------------------------------------------------------------------
bool are_restricted_disjoint( const Variable *variable_0,
    const Variable *variable_1,
    const std::vector<const Disjoint_variable_restriction *> &restrictions )
{
    for( auto restriction : restrictions )
    {
        auto restricted_0 = restriction->get_expression().at(0);
        auto restricted_1 = restriction->get_expression().at(1);
        if( ( restricted_0 == variable_0 && restricted_1 == variable_1 ) ||
            ( restricted_1 == variable_0 && restricted_0 == variable_1 ) )
        {
            return true;
        }
    }
    return false;
}
//------------------------------------------------------------------------------
bool verify_disjoint_variable_restriction( const Expression &expression_0,
    const Expression &expression_1,
    const std::vector<const Disjoint_variable_restriction *> &restrictions )
{
    for( auto symbol_0 : expression_0 )
    {
        for( auto symbol_1 : expression_1 )
        {
            auto variable_0 = symbol_cast<const Variable *>( symbol_0 );
            auto variable_1 = symbol_cast<const Variable *>( symbol_1 );
            if( variable_0 && variable_1 )
            {
                if( variable_0 == variable_1 )
                {
                    return false;
                }
                else
                {
                    const bool result = are_restricted_disjoint( variable_0,
                        variable_1, restrictions );
                    if( !result )
                        return false;
                }

            }
        }
    }
    return true;
}
//------------------------------------------------------------------------------
bool verify_disjoint_variable_restrictions(
    const Substitution_map &substitution_map,
    const std::vector<const Disjoint_variable_restriction *> &restrictions )
{
    for( auto &substitution_0 : substitution_map )
    {
        for( auto &substitution_1 : substitution_map )
        {
            auto variable_0 = substitution_0.first;
            auto variable_1 = substitution_1.first;
            for( auto &restriction : restrictions )
            {
                if( variable_0 == restriction->get_expression().at(0) &&
                    variable_1 == restriction->get_expression().at(1) )
                {
                    const bool result = verify_disjoint_variable_restriction(
                        substitution_0.second, substitution_1.second,
                        restrictions );
                    if( !result )
                        return false;
                }
            }
        }
    }
    return true;
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
        const auto substitution_range = frame.get_statements().size() - 1;
        assert( m_stack.size() >= substitution_range );
        std::vector<Expression> substitution_effect(
            m_stack.end() - substitution_range, m_stack.end() );
        std::vector<const Statement *> substitution_base(
            frame.get_statements().begin(), frame.get_statements().end() - 1 );
        m_result = fill_substitution_map( substitution_map, substitution_effect,
            substitution_base );
        if( !m_result )
            return;
        m_result = verify_disjoint_variable_restrictions( substitution_map,
            frame.get_restrictions() );
        if( !m_result )
            return;
        m_stack.resize( m_stack.size() - substitution_range );

        m_stack.push_back( Expression() );
        apply_substitution( substitution_map, assertion->get_expression(),
            m_stack.back() );
    }
    void push_hypothesis( const Statement *statement,
        const Expression &expression )
    {
        auto available_statements = m_frame.get_statements();
        const bool available = std::find( available_statements.begin(),
            available_statements.end(), statement ) !=
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
            return false;
        }
    }
    return stack.get_top() == theorem->get_expression();
}
//------------------------------------------------------------------------------
class Statement_verifier : public Lazy_const_statement_visitor
{
public:
    void operator()( const Scoping_statement *scoping_statement ) override
    {
        auto statement = scoping_statement->get_first();
        while( statement )
        {
            statement->accept( *this );
            if( !m_result )
                break;
            statement = statement->get_next();
        }
    }
    void operator()( const Theorem *theorem ) override
    {
        m_result = verify( theorem );
    }
    bool operator()()
    {
        return m_result;
    }
private:
    bool m_result = true;
};
//------------------------------------------------------------------------------
bool verify( const Metamath_database &db )
{
    Statement_verifier verifier;
    db.get_top_scope()->accept( verifier );
    return verifier();
}
