#ifndef STATEMENT_H
#define STATEMENT_H

#include <vector>
#include <list>

#include "Symbol.h"
#include "Proof_step.h"

class Scoping_statement;
class Constant_declaration;
class Variable_declaration;
class Axiom;
class Theorem;
class Essential_hypothesis;
class Floating_hypothesis;
class Disjoint_variable_restriction;
class Proof_step;
//------------------------------------------------------------------------------
class Statement_visitor
{
public:
    virtual void operator()( Scoping_statement * ) = 0;
    virtual void operator()( Constant_declaration * ) = 0;
    virtual void operator()( Variable_declaration * ) = 0;
    virtual void operator()( Axiom * ) = 0;
    virtual void operator()( Theorem * ) = 0;
    virtual void operator()( Essential_hypothesis * ) = 0;
    virtual void operator()( Floating_hypothesis * ) = 0;
    virtual void operator()( Disjoint_variable_restriction * ) = 0;
};
//------------------------------------------------------------------------------
class Const_statement_visitor
{
public:
    virtual void operator()( const Scoping_statement * ) = 0;
    virtual void operator()( const Constant_declaration * ) = 0;
    virtual void operator()( const Variable_declaration * ) = 0;
    virtual void operator()( const Axiom * ) = 0;
    virtual void operator()( const Theorem * ) = 0;
    virtual void operator()( const Essential_hypothesis * ) = 0;
    virtual void operator()( const Floating_hypothesis * ) = 0;
    virtual void operator()( const Disjoint_variable_restriction * ) = 0;
};
//------------------------------------------------------------------------------
class Statement
{
protected:
    Statement()
    { }
public:
    virtual ~Statement()
    { }
    Statement *get_previous()
    {
        return m_previous;
    }
    Statement *get_next()
    {
        return m_next;
    }
    const Statement *get_previous() const
    {
        return m_previous;
    }
    const Statement *get_next() const
    {
        return m_next;
    }
    void set_previous( Statement *previous )
    {
        m_previous = previous;
    }
    void set_next( Statement *next )
    {
        m_next = next;
    }
    virtual void accept( Statement_visitor &visitor ) = 0;
    virtual void accept( Const_statement_visitor &visitor ) const = 0;
private:
    Statement *m_previous = 0;
    Statement *m_next = 0;
};
//------------------------------------------------------------------------------
class Named_statement : public Statement, public Named
{
protected:
    Named_statement( const std::string &label ):
        Named( label )
    { }
};
//------------------------------------------------------------------------------
typedef std::vector<const Symbol *> Expression;
//------------------------------------------------------------------------------
class Expression_holder
{
public:
    Expression &get_expression()
    {
        return m_expression;
    }
    const Expression &get_expression() const
    {
        return m_expression;
    }
private:
    Expression m_expression;
};
//------------------------------------------------------------------------------
class Constant_declaration :
    public Statement,
    public Expression_holder
{
public:
    Constant_declaration()
    { }
    ~Constant_declaration();
    void accept( Statement_visitor &visitor ) override
    {
        visitor( this );
    }
    void accept( Const_statement_visitor &visitor ) const override
    {
        visitor( this );
    }
};
//------------------------------------------------------------------------------
class Variable_declaration :
    public Statement,
    public Expression_holder
{
public:
    Variable_declaration()
    { }
    ~Variable_declaration();
    void accept( Statement_visitor &visitor ) override
    {
        visitor( this );
    }
    void accept( Const_statement_visitor &visitor ) const override
    {
        visitor( this );
    }
};
//------------------------------------------------------------------------------
class Assertion :
    public Named_statement,
    public Expression_holder
{
public:
    Assertion( const std::string &label ) :
        Named_statement( label )
    { }
};
//------------------------------------------------------------------------------
class Axiom :
    public Assertion
{
public:
    Axiom( const std::string &label ) :
        Assertion( label )
    { }
    void accept( Statement_visitor &visitor ) override
    {
        visitor( this );
    }
    void accept( Const_statement_visitor &visitor ) const override
    {
        visitor( this );
    }
};
//------------------------------------------------------------------------------
class Proof
{
public:
    typedef std::vector<const Proof_step *> Steps;
    const Steps &get_steps() const
    {
        return m_steps;
    }
    Steps &get_steps()
    {
        return m_steps;
    }
    void set_compressed( bool compressed )
    {
        m_compressed = compressed;
    }
    bool get_compressed() const
    {
        return m_compressed;
    }
    ~Proof()
    {
        for( auto step : m_steps )
            delete step;
    }

private:
    Steps m_steps;
    bool m_compressed = false;
};
//------------------------------------------------------------------------------
class Theorem :
    public Assertion
{
public:
    Theorem( const std::string &label ) :
        Assertion( label )
    { }
    void accept( Statement_visitor &visitor ) override
    {
        visitor( this );
    }
    void accept( Const_statement_visitor &visitor ) const override
    {
        visitor( this );
    }
    Proof &get_proof()
    {
        return m_proof;
    }
    const Proof &get_proof() const
    {
        return m_proof;
    }
private:
    Proof m_proof;
};
//------------------------------------------------------------------------------
class Essential_hypothesis :
    public Named_statement,
    public Expression_holder
{
public:
    Essential_hypothesis( const std::string &label ) :
        Named_statement( label )
    { }
    void accept( Statement_visitor &visitor ) override
    {
        visitor( this );
    }
    void accept( Const_statement_visitor &visitor ) const override
    {
        visitor( this );
    }
};
//------------------------------------------------------------------------------
class Floating_hypothesis :
    public Named_statement,
    public Expression_holder
{
public:
    Floating_hypothesis( const std::string &label ) :
        Named_statement( label )
    { }
    void accept( Statement_visitor &visitor ) override
    {
        visitor( this );
    }
    void accept( Const_statement_visitor &visitor ) const override
    {
        visitor( this );
    }
};
//------------------------------------------------------------------------------
class Disjoint_variable_restriction :
    public Statement,
    public Expression_holder
{
public:
    Disjoint_variable_restriction()
    { }
    void accept( Statement_visitor &visitor ) override
    {
        visitor( this );
    }
    void accept( Const_statement_visitor &visitor ) const override
    {
        visitor( this );
    }
};

#endif // STATEMENT_H
