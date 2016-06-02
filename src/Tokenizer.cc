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
#include "Tokenizer.h"
#include <stdexcept>

Tokenizer::Tokenizer( std::istream &input_stream ) :
    m_input_stream( input_stream )
{
    m_input_stream >> m_next_token;
}
//------------------------------------------------------------------------------
std::string Tokenizer::get_token()
{
    if( m_next_token.empty() )
    {
        throw( std::runtime_error("requested a token from past the end of the"
            "stream") );
    }
    std::string result( m_next_token );
    extract_next_token();
    return result;
}
//------------------------------------------------------------------------------
const std::string &Tokenizer::peek()
{
    return m_next_token;
}
//------------------------------------------------------------------------------
void Tokenizer::extract_next_token()
{
    m_input_stream >> m_next_token;
    if(!m_input_stream)
        m_next_token.clear();
}
