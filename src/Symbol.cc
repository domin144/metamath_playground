#include "Symbol.h"

Symbol::Symbol( const std::string &name ) :
  m_name( name )
{ }
//------------------------------------------------------------------------------
const std::string &Symbol::get_name() const {
  return m_name;
}
//------------------------------------------------------------------------------
bool Symbol::operator<( const Symbol &other_symbol ) const {
  return get_name() < other_symbol.get_name();
}
//------------------------------------------------------------------------------
Variable::Variable( const std::string &name ) :
  Symbol( name )
{ }
//------------------------------------------------------------------------------
Constant::Constant( const std::string &name ) :
  Symbol( name )
{ }
