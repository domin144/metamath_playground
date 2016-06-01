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
#ifndef STATEMENT_VISITORS_H
#define STATEMENT_VISITORS_H

#include "Statement.h"

class Lazy_statement_visitor : public Statement_visitor
{
public:
    void operator()( Scoping_statement * ) override
    { }
    void operator()( Constant_declaration * ) override
    { }
    void operator()( Variable_declaration * ) override
    { }
    void operator()( Axiom * ) override
    { }
    void operator()( Theorem * ) override
    { }
    void operator()( Essential_hypothesis * ) override
    { }
    void operator()( Floating_hypothesis * ) override
    { }
    void operator()( Disjoint_variable_restriction * ) override
    { }
};
//------------------------------------------------------------------------------
class Lazy_const_statement_visitor : public Const_statement_visitor
{
public:
    void operator()( const Scoping_statement * ) override
    { }
    void operator()( const Constant_declaration * ) override
    { }
    void operator()( const Variable_declaration * ) override
    { }
    void operator()( const Axiom * ) override
    { }
    void operator()( const Theorem * ) override
    { }
    void operator()( const Essential_hypothesis * ) override
    { }
    void operator()( const Floating_hypothesis * ) override
    { }
    void operator()( const Disjoint_variable_restriction * ) override
    { }
};
//------------------------------------------------------------------------------
template<class Statement_pointer>
class Statement_caster : public Lazy_statement_visitor
{
public:
    Statement_caster( Statement *statement )
    {
        statement->accept( *this );
    }
    void operator()( Statement_pointer statement ) override
    {
        m_statement = statement;
    }
    operator Statement_pointer()
    {
        return m_statement;
    }
    operator bool()
    {
        return m_statement;
    }

private:
    Statement_pointer m_statement = 0;
};
//------------------------------------------------------------------------------
template<class Statement_pointer>
class Const_statement_caster : public Lazy_const_statement_visitor
{
public:
    Const_statement_caster( const Statement *statement )
    {
        statement->accept( *this );
    }
    void operator()( Statement_pointer statement ) override
    {
        m_statement = statement;
    }
    operator Statement_pointer()
    {
        return m_statement;
    }
    operator bool()
    {
        return m_statement;
    }

private:
    Statement_pointer m_statement = 0;
};
//------------------------------------------------------------------------------
template<>
class Statement_caster<Assertion *> : public Lazy_statement_visitor
{
public:
    Statement_caster( Statement *statement )
    {
        statement->accept( *this );
    }
    void operator()( Axiom *axiom ) override
    {
        m_statement = axiom;
    }
    void operator()( Theorem *theorem ) override
    {
        m_statement = theorem;
    }
    operator Assertion *()
    {
        return m_statement;
    }
    operator bool()
    {
        return m_statement;
    }

private:
    Assertion *m_statement = 0;
};
//------------------------------------------------------------------------------
template<>
class Const_statement_caster<const Assertion *> :
    public Lazy_const_statement_visitor
{
public:
    Const_statement_caster( const Statement *statement )
    {
        statement->accept( *this );
    }
    void operator()( const Axiom *axiom ) override
    {
        m_statement = axiom;
    }
    void operator()( const Theorem *theorem ) override
    {
        m_statement = theorem;
    }
    operator const Assertion *()
    {
        return m_statement;
    }
    operator bool()
    {
        return m_statement;
    }

private:
    const Assertion *m_statement = 0;
};
//------------------------------------------------------------------------------
template<class Statement_pointer>
Statement_pointer statement_cast( Statement *statement )
{
    return Statement_caster<Statement_pointer>( statement );
}
//------------------------------------------------------------------------------
template<class Statement_pointer>
Statement_pointer statement_cast( const Statement *statement )
{
    return Const_statement_caster<Statement_pointer>( statement );
}

#endif // STATEMENT_VISITOR_H
