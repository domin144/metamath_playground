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
#ifndef SCOPE_H
#define SCOPE_H

#include "Statement.h"
#include <map>

class Scoping_statement :
    public Statement
{
public:
    Scoping_statement( Scoping_statement *parrent=0 );
    ~Scoping_statement();
    bool is_top() const;
    const Symbol *get_symbol_by_label( const std::string &label ) const;
    const Named_statement *get_statement_by_label( const std::string &label );
    void add_statement( Statement *statement );
    void register_symbol( const Symbol *symbol );
    Scoping_statement *get_parrent();
    Statement *get_first();
    const Statement *get_first() const;
    void accept( Statement_visitor &visitor ) override
    {
        visitor( this );
    }
    void accept( Const_statement_visitor &visitor ) const override
    {
        visitor( this );
    }
private:
    Scoping_statement *m_parrent = 0;
    Statement *m_first = 0;
    Statement *m_last = 0;

    std::map< std::string, const Symbol *> m_label_to_symbol;
    std::map< std::string, const Named_statement *> m_label_to_statement;
};

#endif // SCOPE_H
