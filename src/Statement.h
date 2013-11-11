#ifndef STATEMENT_H
#define STATEMENT_H

#include <vector>

#include "Symbol.h"

class Statement : public Named
{
protected:
    Statement( const std::string &name );
public:
    void push_back( const Symbol &symbol );
private:
    std::vector<const Symbol *> m_content;
};
//------------------------------------------------------------------------------
class Axiom : public Statement
{
public:
    Axiom( const std::string &name );
};
//------------------------------------------------------------------------------
class Theorem: public Statement
{
public:
    Theorem( const std::string &name );
};
//------------------------------------------------------------------------------
class Assumption : public Statement
{
public:
    Assumption( const std::string &name );
};
//------------------------------------------------------------------------------
class Variable_assumption : public Statement
{
public:
    Variable_assumption( const std::string &name );
};

#endif // STATEMENT_H
