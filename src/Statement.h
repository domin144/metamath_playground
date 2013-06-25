#ifndef STATEMENT_H
#define STATEMENT_H

#include <vector>

#include "Symbol.h"

class Statement {
protected:
  Statement();
public:
  const std::string &get_name() const;
  void push_back( const Symbol &symbol );
private:
  std::string m_name;
  std::vector<const Symbol *> m_content;
};
//------------------------------------------------------------------------------
class Axiom : public Statement {
};
//------------------------------------------------------------------------------
class Theorem: public Statement {
};
//------------------------------------------------------------------------------
class Assumption : public Statement {
};
//------------------------------------------------------------------------------
class Variable_assumption : public Statement {
};
//------------------------------------------------------------------------------
bool operator<( const Statement &left_statement, const Statement
  &right_statement );

#endif // STATEMENT_H
