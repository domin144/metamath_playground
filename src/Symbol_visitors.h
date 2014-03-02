#ifndef SYMBOL_VISITORS_H
#define SYMBOL_VISITORS_H

#include "Symbol.h"

class Lazy_symbol_visitor : public Symbol_visitor
{
public:
    void operator()( Constant * ) override
    { }
    void operator()( Variable * ) override
    { }
};
//------------------------------------------------------------------------------
class Lazy_const_symbol_visitor : public Const_symbol_visitor
{
public:
    void operator()( const Constant * ) override
    { }
    void operator()( const Variable * ) override
    { }
};
//------------------------------------------------------------------------------
template<class Symbol_pointer>
class Symbol_caster : public Lazy_symbol_visitor
{
public:
    Symbol_caster( Symbol *symbol )
    {
        symbol->accept( *this );
    }
    void operator()( Symbol_pointer symbol ) override
    {
        m_symbol = symbol;
    }
    operator Symbol_pointer()
    {
        return m_symbol;
    }
    operator bool()
    {
        return m_symbol;
    }

private:
    Symbol_pointer m_symbol = 0;
};
//------------------------------------------------------------------------------
template<class Symbol_pointer>
class Const_symbol_caster : public Lazy_const_symbol_visitor
{
public:
    Const_symbol_caster( const Symbol *symbol )
    {
        symbol->accept( *this );
    }
    void operator()( Symbol_pointer symbol ) override
    {
        m_symbol = symbol;
    }
    operator Symbol_pointer()
    {
        return m_symbol;
    }
    operator bool()
    {
        return m_symbol;
    }

private:
    Symbol_pointer m_symbol = 0;
};
//------------------------------------------------------------------------------
template<class Symbol_pointer>
Symbol_pointer symbol_cast( Symbol *symbol )
{
    return Symbol_caster<Symbol_pointer>( symbol );
}
//------------------------------------------------------------------------------
template<class Symbol_pointer>
Symbol_pointer symbol_cast( const Symbol *symbol )
{
    return Const_symbol_caster<Symbol_pointer>( symbol );
}

#endif // SYMBOL_VISITORS_H
