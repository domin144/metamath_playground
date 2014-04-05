#include "Statement.h"

Constant_declaration::~Constant_declaration()
{
    while( !get_expression().empty() )
    {
        delete get_expression().back();
        get_expression().pop_back();
    }
}
//------------------------------------------------------------------------------
Variable_declaration::~Variable_declaration()
{
    while( !get_expression().empty() )
    {
        delete get_expression().back();
        get_expression().pop_back();
    }
}
