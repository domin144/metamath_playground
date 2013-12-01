#ifndef STATEMENT_H
#define STATEMENT_H

#include <vector>
#include <list>

#include "Symbol.h"

class Statement
{
protected:
    Statement();
public:
    virtual ~Statement();
    enum class Type
    {
        constant_declaration,
        variable_declaration,
        axiom,
        theorem,
        essential_hypothesis,
        floating_hypothesis,
        disjoint_variable_restriction,
        scoping_statement
    };
    virtual Type get_type() const = 0;
    Statement *get_previous();
    Statement *get_next();
    void set_previous( Statement *previous );
    void set_next( Statement *next );
    std::vector<Symbol *> &get_expression();
private:
    std::vector<Symbol *> m_expression;
    Statement *m_previous = 0;
    Statement *m_next = 0;
};
//------------------------------------------------------------------------------
class Named_statement : public Statement, public Named
{
public:
    Named_statement( const std::string &label ):
        Named( label )
    { }
};
//------------------------------------------------------------------------------
template<Statement::Type type, typename TBase_type>
class Typed_statement : public TBase_type
{
public:
    typedef TBase_type Base_type;
    Typed_statement()
    { }
    Typed_statement( const std::string &label ) :
        TBase_type( label )
    { }
    typename TBase_type::Type get_type() const
    {
        return type;
    }
};
//------------------------------------------------------------------------------
class Constant_declaration :
    public Typed_statement<Statement::Type::constant_declaration, Statement>
{
public:
    Constant_declaration();
    ~Constant_declaration();
};
//------------------------------------------------------------------------------
class Variable_declaration :
    public Typed_statement<Statement::Type::variable_declaration, Statement>
{
public:
    Variable_declaration();
    ~Variable_declaration();
};
//------------------------------------------------------------------------------
class Axiom :
    public Typed_statement<Statement::Type::axiom, Named_statement>
{
public:
    Axiom( const std::string &label );
};
//------------------------------------------------------------------------------
class Theorem :
    public Typed_statement<Statement::Type::theorem, Named_statement>
{
public:
    Theorem( const std::string &label );
    std::vector<Named_statement *> &get_proof();
private:
    std::vector<Named_statement *> m_proof;
};
//------------------------------------------------------------------------------
class Essential_hypothesis :
    public Typed_statement<Statement::Type::essential_hypothesis,
        Named_statement>
{
public:
    Essential_hypothesis( const std::string &label );
};
//------------------------------------------------------------------------------
class Floating_hypothesis :
    public Typed_statement<Statement::Type::floating_hypothesis,
        Named_statement>
{
public:
    Floating_hypothesis( const std::string &label );
};
//------------------------------------------------------------------------------
class Disjoint_variable_restriction :
    public Typed_statement<Statement::Type::disjoint_variable_restriction,
        Statement>
{
public:
    Disjoint_variable_restriction();
};

#endif // STATEMENT_H
