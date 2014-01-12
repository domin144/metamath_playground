#ifndef STATEMENT_H
#define STATEMENT_H

#include <vector>
#include <list>
#include <boost/ptr_container/ptr_vector.hpp>

#include "Symbol.h"

class Scoping_statement;
class Constant_declaration;
class Variable_declaration;
class Axiom;
class Theorem;
class Essential_hypothesis;
class Floating_hypothesis;
class Disjoint_variable_restriction;
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
    Statement();
public:
    virtual ~Statement();
    Statement *get_previous();
    Statement *get_next();
    const Statement *get_previous() const;
    const Statement *get_next() const;
    void set_previous( Statement *previous );
    void set_next( Statement *next );
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
typedef boost::ptr_vector< Symbol, boost::view_clone_allocator >
    Expression;
//------------------------------------------------------------------------------
class Expression_holder
{
public:
    Expression &get_expression();
    const Expression &get_expression() const;
private:
    Expression m_expression;
};
//------------------------------------------------------------------------------
class Constant_declaration :
    public Statement,
    public Expression_holder
{
public:
    Constant_declaration();
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
    Variable_declaration();
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
    Axiom( const std::string &label );
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
typedef std::vector<const Named_statement *> Proof;
//------------------------------------------------------------------------------
class Theorem :
    public Assertion
{
public:
    Theorem( const std::string &label );
    void accept( Statement_visitor &visitor ) override
    {
        visitor( this );
    }
    void accept( Const_statement_visitor &visitor ) const override
    {
        visitor( this );
    }
    Proof &get_proof();
    const Proof &get_proof() const;
private:
    Proof m_proof;
};
//------------------------------------------------------------------------------
class Essential_hypothesis :
    public Named_statement,
    public Expression_holder
{
public:
    Essential_hypothesis( const std::string &label );
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
    Floating_hypothesis( const std::string &label );
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
    Disjoint_variable_restriction();
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
