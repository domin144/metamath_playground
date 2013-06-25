#include <stdexcept>
#include "Metamath_database.h"

Metamath_database::Metamath_database() {
}
//------------------------------------------------------------------------------
const Statement &Metamath_database::get_statement_by_label( const std::string
  &label )
{
  return *m_label_to_statement.at( label );
}
//------------------------------------------------------------------------------
const Symbol &Metamath_database::get_symbol_by_label( const std::string &label )
{
  return *m_label_to_symbol.at( label );
}
//------------------------------------------------------------------------------
void Metamath_database::add_axiom( const Axiom &axiom ) {
  const Axiom *new_axiom = &*( m_axioms.insert( axiom ).first );
  m_label_to_statement[new_axiom->get_name()] = new_axiom;
}
//------------------------------------------------------------------------------
void Metamath_database::add_variable( const Variable &variable ) {
  const Variable *new_variable = &*( m_variables.insert( variable ).first );
  m_label_to_symbol[new_variable->get_name()] = new_variable;
}
//------------------------------------------------------------------------------
void Metamath_database::add_constant( const Constant &constant ) {
  const Constant *new_constant = &*( m_constants.insert( constant ).first );
  m_label_to_symbol[new_constant->get_name()] = new_constant;
}
//------------------------------------------------------------------------------
void Metamath_database::add_variable_assumption( const Variable_assumption
  &variable_assumption )
{
  const Variable_assumption *new_variable_assumption = &*(
    m_variable_assumptions.insert( variable_assumption ).first );
  m_label_to_statement[new_variable_assumption->get_name()] =
    new_variable_assumption;
}
//------------------------------------------------------------------------------
void Metamath_database::add_assumption( const Assumption &assumption ) {
  const Assumption *new_assumption = &*(
    m_assumptions.insert( assumption ).first );
  m_label_to_statement[new_assumption->get_name()] = new_assumption;
}
//------------------------------------------------------------------------------
void Metamath_database::add_theorem( const Theorem &theorem ) {
  const Theorem *new_theorem = &*(
    m_theorems.insert( theorem ).first );
  m_label_to_statement[new_theorem->get_name()] = new_theorem;
}
//------------------------------------------------------------------------------
const Statement *Metamath_database::getStatementByName( const std::string &name ) {
  return m_label_to_statement.at( name );
}
