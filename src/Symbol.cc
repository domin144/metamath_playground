#include "Symbol.h"

Symbol::Symbol( const std::string &name ) :
    Named( name )
{ }
//------------------------------------------------------------------------------
Variable::Variable( const std::string &name ) :
    Symbol( name )
{ }
//------------------------------------------------------------------------------
Constant::Constant( const std::string &name ) :
    Symbol( name )
{ }
