#ifndef EXPRESSION_H
#define EXPRESSION_H
#include "Symbol.h"

class Expression {
public:
  std::vector<Symbol *> &symbols();
private:
  std::vector<Symbol *> m_symbols;
};

#endif // EXPRESSION_H
