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
#ifndef NAMED_H
#define NAMED_H

#include <string>

class Named
{
public:
    Named( const std::string &name );
    const std::string &get_name() const;
private:
    std::string m_name;
};
//------------------------------------------------------------------------------
class Less_by_name
{
public:
    bool operator()( const Named &lhs, const Named &rhs ) const;
};

#endif // NAMED_H
