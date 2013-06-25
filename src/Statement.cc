#include "Statement.h"

Statement::Statement() {
}
//------------------------------------------------------------------------------
const std::string &Statement::get_name() const {
  return m_name;
}
//------------------------------------------------------------------------------
void Statement::push_back( const Symbol &symbol ) {
  m_content.push_back( &symbol );
}
//------------------------------------------------------------------------------
bool operator<( const Statement &left_statement, const Statement
  &right_statement )
{
  return left_statement.get_name() < right_statement.get_name();
}
