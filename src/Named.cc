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
#include "Named.h"

Named::Named( const std::string &name ) :
    m_name( name )
{ }
//------------------------------------------------------------------------------
const std::string &Named::get_name() const
{
    return m_name;
}
//------------------------------------------------------------------------------
bool Less_by_name::operator()( const Named &lhs, const Named &rhs ) const
{
    return lhs.get_name() < rhs.get_name();
}
