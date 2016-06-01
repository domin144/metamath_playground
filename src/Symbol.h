/* 
 * Copyright 2013 Dominik Wójt
 * 
 * This file is part of metamath_playground.
 * 
 * metamath_playground is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * metamath_playground is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with metamath_playground.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
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
    Symbol( const std::string &name ) :
        Named( name )
    { }
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
    Variable( const std::string &name ) :
        Symbol( name )
    { }
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
    Constant( const std::string &name ) :
        Symbol( name )
    { }
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
