#ifndef METAMATH_DATABASE
#define METAMATH_DATABASE

#include <iostream>
#include <set>
#include <map>

#include "Statement.h"
#include "Symbol.h"

class Metamath_database {
public:
  Metamath_database();
  const Statement &get_statement_by_label( const std::string &label );
  const Symbol &get_symbol_by_label( const std::string &label );
  void add_axiom( const Axiom &axiom );
  void add_variable( const Variable &variable );
  void add_constant( const Constant &constant );
  void add_variable_assumption( const Variable_assumption &variable_assumption
    );
  void add_assumption( const Assumption &assumption );
  void add_theorem( const Theorem &theorem );
  const Statement *getStatementByName( const std::string &name );
private:
  std::set<Axiom> m_axioms;
  std::set<Theorem> m_theorems;
  std::set<Variable> m_variables;
  std::set<Constant> m_constants;
  std::set<Assumption> m_assumptions;
  std::set<Variable_assumption> m_variable_assumptions;
  std::map<std::string, const Statement *> m_label_to_statement;
  std::map<std::string, const Symbol *> m_label_to_symbol;
  Metamath_database *const parrent = 0;
};

#endif // METAMATH_DATABASE
