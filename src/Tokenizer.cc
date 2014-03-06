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
    try
    {
        m_input_stream >> m_next_token;
    }
    catch( std::ios_base::failure )
    {
        m_next_token.clear();
    }
}
