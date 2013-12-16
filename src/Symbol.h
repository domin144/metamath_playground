#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include "Named.h"

class Constant;
class Variable;
//------------------------------------------------------------------------------
class Symbol_visitor
{
public:
    virtual void operator()( Constant * ) = 0;
    virtual void operator()( Variable * ) = 0;
};
//------------------------------------------------------------------------------
class Const_symbol_visitor
{
public:
    virtual void operator()( const Constant * ) = 0;
    virtual void operator()( const Variable * ) = 0;
};
//------------------------------------------------------------------------------
class Symbol : public Named
{
protected:
    Symbol( const std::string &name );
public:
    virtual ~Symbol()
    { }
    virtual void accept( Symbol_visitor &visitor ) = 0;
    virtual void accept( Const_symbol_visitor &visitor ) const = 0;
};
//------------------------------------------------------------------------------
class Variable : public Symbol
{
public:
    Variable( const std::string &name );
    void accept( Symbol_visitor &visitor ) override
    {
        visitor( this );
    }
    void accept( Const_symbol_visitor &visitor ) const override
    {
        visitor( this );
    }
};
//------------------------------------------------------------------------------
class Constant : public Symbol
{
public:
    Constant( const std::string &name );
    void accept( Symbol_visitor &visitor ) override
    {
        visitor( this );
    }
    void accept( Const_symbol_visitor &visitor ) const override
    {
        visitor( this );
    }
};

#endif // SYMBOL_H
