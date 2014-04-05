#include <set>
#include <algorithm>
#include <cassert>
#include <stack>
#include <sstream>

#include "Statement_visitors.h"
#include "Symbol_visitors.h"
#include "verify.h"

//#define VERIFIER_DEBUG
#ifdef VERIFIER_DEBUG
#include <iostream>
void print_expression( const int i, const std::string &name,
    const Expression &expression )
{
    std::cout << i << ", " << name << ": ";
    for( auto symbol : expression )
    {
        std::cout << symbol->get_name() << ' ';
    }
    std::cout << '\n';
}
#endif

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
    if( !assertion )
        throw std::runtime_error( "last statement in a frame is not an "
            "assertion" );
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
                if( !symbol_cast<const Variable *>( variable ) )
                    throw std::runtime_error( "second symbol in a floating "
                        "hypothesis is not a variable" );
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
        m_base( base ),
        m_effect_iterator( effect.begin() )
    {
        if( base.size() != effect.size() )
        {
            throw( verification_failure( "count of base expression does not "
                "match effect statesments count" ) );
        }
    }
    void collect()
    {
        for( auto statement : m_base )
        {
            statement->accept( *this );
            ++m_effect_iterator;
        }
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
            std::stringstream ss;
            ss <<  "substitution in an essential hypothesis \"" <<
                hypothesis->get_name() << "\" does not match one deducted from "
                "a floating hyptheses";
            throw verification_failure( ss.str() );
        }
    }
    void operator()( const Floating_hypothesis *hypothesis ) override
    {
        auto &effect_expression = *m_effect_iterator;
        auto &base_expression = hypothesis->get_expression();

        // add substitution for variable
        if( base_expression[0] != effect_expression[0] )
        {
            std::stringstream ss;
            ss << "floating hypthesis \"" << hypothesis->get_name() << "\" does"
                " not match expression in substitution";
            throw verification_failure( ss.str() );
        }

        if( m_map.find( base_expression[1] ) != m_map.end() )
        {
            std::stringstream ss;
            ss << "floating hypthesis \"" << hypothesis->get_name() <<
                "\" found twice while composing substitution";
            throw verification_failure( ss.str() );
        }

        m_map[base_expression[1]].assign(
            ++effect_expression.begin(), effect_expression.end() );
    }

    Substitution_map &m_map;
    const std::vector<const Statement *> &m_base;
    std::vector<Expression>::const_iterator m_effect_iterator;
};
//------------------------------------------------------------------------------
void fill_substitution_map( Substitution_map &map,
    const std::vector<Expression> &effect,
    const std::vector<const Statement *> &base )
{
    Substitution_collector substitution_collector( map, effect, base );
    substitution_collector.collect();
}
//------------------------------------------------------------------------------
bool are_restricted_disjoint( const Variable *variable_0,
    const Variable *variable_1,
    const std::vector<const Disjoint_variable_restriction *> &restrictions )
{
    for( auto restriction : restrictions )
    {
        const auto &expression = restriction->get_expression();
        bool found_0 =
            std::find( expression.begin(), expression.end(), variable_0 ) !=
            expression.end();
        bool found_1 =
            std::find( expression.begin(), expression.end(), variable_1 ) !=
            expression.end();
        if( found_0 && found_1 )
        {
            return true;
        }
    }
    return false;
}
//------------------------------------------------------------------------------
void verify_disjoint_variable_restriction( const Expression &expression_0,
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
                    std::stringstream ss;
                    ss << "same variable \"" << variable_0->get_name() <<
                        "\" in both expressions";
                    throw verification_failure( ss.str() );
                }
                else
                {
                    const bool result = are_restricted_disjoint( variable_0,
                        variable_1, restrictions );
                    if( !result )
                    {
                        std::stringstream ss;
                        ss << "no distinct variable restriction found to "
                            "guarantee, that variables \"" <<
                            variable_0->get_name() << "\" and \"" <<
                            variable_1->get_name() << "\" stay distinct";
                        throw verification_failure( ss.str() );
                    }
                }
            }
        }
    }
}
//------------------------------------------------------------------------------
void verify_disjoint_variable_restrictions(
    const Substitution_map &substitution_map,
    const std::vector<const Disjoint_variable_restriction *> &restrictions )
{
    for( auto restriction : restrictions )
    {
        if( restriction->get_expression().size() > 2 )
            throw verification_failure( "TODO: handle distinct variable "
                "restrictions with more than two variables" );
        for( auto &substitution_0 : substitution_map )
        {
            for( auto &substitution_1 : substitution_map )
            {
                auto variable_0 = substitution_0.first;
                auto variable_1 = substitution_1.first;
                if( variable_0 == restriction->get_expression().at(0) &&
                    variable_1 == restriction->get_expression().at(1) )
                {
                    verify_disjoint_variable_restriction(
                        substitution_0.second, substitution_1.second,
                        restrictions );
                }
            }
        }
    }
}
//------------------------------------------------------------------------------
class Proof_stack :
    private Const_proof_step_visitor
{
public:
    Proof_stack( const Frame &frame ) :
        m_frame( frame )
    { }
    const Expression &get_top() const
    {
        return m_stack.back();
    }
    void push( const Proof_step *step )
    {
        step->accept( *this );
    }

private:
    void operator()( const Assertion_step *step ) override
    {
        push_assertion( step->get_assertion() );
    }
    void operator()( const Essential_hypothesis_step *step ) override
    {
        auto hypothesis = step->get_hypothesis();
        push_hypothesis( hypothesis, hypothesis->get_expression() );
    }
    void operator()( const Floating_hypothesis_step *step ) override
    {
        auto hypothesis = step->get_hypothesis();
        push_hypothesis( hypothesis, hypothesis->get_expression() );
    }
    void operator()( const Add_reference_step * ) override
    {
        throw std::runtime_error( "not implemented yet" );
    }
    void operator()( const Refer_step * ) override
    {
        throw std::runtime_error( "not implemented yet" );
    }
    void operator()( const Unknown_step * ) override
    {
        throw verification_failure( "missing proof step found" );
    }
    void push_assertion( const Assertion *assertion )
    {
        if( assertion->get_name() == "?" )
            throw verification_failure( "missing proof step found" );

        Frame frame( assertion );
        collect_frame( frame );

        Substitution_map substitution_map;
        const auto substitution_range = frame.get_statements().size() - 1;
        if( m_stack.size() < substitution_range )
            throw verification_failure( "not enough expressions on stack found"
                " to make a substitution" );
        std::vector<Expression> substitution_effect(
            m_stack.end() - substitution_range, m_stack.end() );
        std::vector<const Statement *> substitution_base(
            frame.get_statements().begin(), frame.get_statements().end() - 1 );
        fill_substitution_map( substitution_map, substitution_effect,
            substitution_base );
        verify_disjoint_variable_restrictions( substitution_map,
            frame.get_restrictions() );  
#ifdef VERIFIER_DEBUG
        std::cout << "using: ";
        const int stack_size = m_stack.size();
        for( int i=stack_size-substitution_range; i<stack_size; i++ )
        {
            std::cout << m_number_stack[i] << ' ';
        }
        std::cout << '\n';
        m_number_stack.resize( m_stack.size()-substitution_range );
#endif
        m_stack.resize( m_stack.size() - substitution_range );

        m_stack.push_back( Expression() );
        apply_substitution( substitution_map, assertion->get_expression(),
            m_stack.back() );
#ifdef VERIFIER_DEBUG
        m_number_stack.push_back( m_pushed_count );
        print_expression( m_pushed_count++, assertion->get_name(),
            m_stack.back() );
#endif
    }
    void push_hypothesis( const Named_statement *statement,
        const Expression &expression )
    {
        auto available_statements = m_frame.get_statements();
        const bool available = std::find( available_statements.begin(),
            available_statements.end(), statement ) !=
            available_statements.end();
        if( !available )
        {
            std::stringstream ss;
            ss << "proof uses hypothesis \"" << statement->get_name() <<
                "\" which is not available in theorem's frame";
            throw verification_failure( ss.str() );
        }
        m_stack.push_back( expression );
#ifdef VERIFIER_DEBUG
        m_number_stack.push_back( m_pushed_count );
        print_expression( m_pushed_count++, statement->get_name(),
            m_stack.back() );
#endif
    }

private:
    const Frame &m_frame;
    std::vector<Expression> m_stack;
#ifdef VERIFIER_DEBUG
    std::vector<int> m_number_stack;
    int m_pushed_count = 0;
#endif
};
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
            statement = statement->get_next();
        }
    }
    void operator()( const Theorem *theorem ) override try
    {
        Frame frame( theorem );
        collect_frame( frame );
        Proof_stack stack( frame );

        for( auto step : theorem->get_proof().get_steps() )
        {
            stack.push( step );
        }

        if( stack.get_top() != theorem->get_expression() )
        {
            throw verification_failure( "proof result does not match theorem "
                "expression" );
        }
    }
    catch( const verification_failure &e )
    {
        std::stringstream ss;
        ss << "failed to verify proof of theorem \"" << theorem->get_name() <<
            "\": " << e.what();
        throw verification_failure( ss.str() );
    }
};
//------------------------------------------------------------------------------
void verify( const Theorem *theorem )
{
    Statement_verifier verifier;
    theorem->accept( verifier );
}
//------------------------------------------------------------------------------
void verify( const Metamath_database &db )
{
    Statement_verifier verifier;
    db.get_top_scope()->accept( verifier );
}
