#ifndef STATEMENT_H
#define STATEMENT_H

#include <vector>
#include <list>

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
class Statement
{
protected:
    Statement();
public:
    virtual ~Statement();
    Statement *get_previous();
    Statement *get_next();
    void set_previous( Statement *previous );
    void set_next( Statement *next );
    virtual void welcome( Statement_visitor &visitor ) = 0;
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
typedef std::vector<Symbol *> Expression;
//------------------------------------------------------------------------------
class Expression_holder
{
public:
    Expression &get_expression();
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
    void welcome( Statement_visitor &visitor ) override
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
    void welcome( Statement_visitor &visitor ) override
    {
        visitor( this );
    }
};
//------------------------------------------------------------------------------
class Axiom :
    public Named_statement,
    public Expression_holder
{
public:
    Axiom( const std::string &label );
    void welcome( Statement_visitor &visitor ) override
    {
        visitor( this );
    }
};
//------------------------------------------------------------------------------
class Theorem :
    public Named_statement,
    public Expression_holder
{
public:
    Theorem( const std::string &label );
    void welcome( Statement_visitor &visitor ) override
    {
        visitor( this );
    }
    std::vector<Named_statement *> &get_proof();
private:
    std::vector<Named_statement *> m_proof;
};
//------------------------------------------------------------------------------
class Essential_hypothesis :
    public Named_statement,
    public Expression_holder
{
public:
    Essential_hypothesis( const std::string &label );
    void welcome( Statement_visitor &visitor ) override
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
    void welcome( Statement_visitor &visitor ) override
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
    void welcome( Statement_visitor &visitor ) override
    {
        visitor( this );
    }
};

#endif // STATEMENT_H
