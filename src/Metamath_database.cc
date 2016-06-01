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
#include <stdexcept>
#include "Metamath_database.h"

Metamath_database::Metamath_database()
{ }
//------------------------------------------------------------------------------
Scoping_statement *Metamath_database::get_top_scope()
{
    return &m_top_scope;
}
//------------------------------------------------------------------------------
const Scoping_statement *Metamath_database::get_top_scope() const
{
    return &m_top_scope;
}
