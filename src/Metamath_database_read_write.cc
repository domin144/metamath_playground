#include <stdexcept>

#include "Metamath_database_read_write.h"

void read_axiom( Metamath_database &db, Tokenizer &tokenizer, const std::string
  &label )
{
  if( tokenizer.get_token() != "$a" )
    throw( std::runtime_error("axiom does not start with \"$a\"") );
  Axiom axiom( label );
  while( tokenizer.peek() != "$." )
    axiom.push_back( db.get_symbol_by_label( tokenizer.get_token() ) );
  db.add_axiom( axiom );
  tokenizer.get_token(); // consume "$."
}
//------------------------------------------------------------------------------
void read_variables( Metamath_database &db, Tokenizer &tokenizer, const
  std::string &label )
{
  if( tokenizer.get_token() != "$v" )
    throw( std::runtime_error("variables do not start with \"$v\"") );

  while( tokenizer.peek() != "$." )
    db.add_variable( tokenizer.get_token() );
  tokenizer.get_token(); // consume "$."
}
//------------------------------------------------------------------------------
void read_scope( Metamath_database &db, Tokenizer &tokenizer, const std::string
  &label )
{
  std::cerr << "Warning: scope read not implemented yet\n";
  while( tokenizer.peek() != "$}" )
    tokenizer.get_token();
  tokenizer.get_token(); // consume "$}"
}
//------------------------------------------------------------------------------
void read_constants( Metamath_database &db, Tokenizer &tokenizer, const
  std::string &label )
{
  if( tokenizer.get_token() != "$c" )
    throw( std::runtime_error("constants do not start with \"$c\"") );
  while( tokenizer.peek() != "$." )
    db.add_constant( tokenizer.get_token() );
  tokenizer.get_token(); // consume "$."
}
//------------------------------------------------------------------------------
void read_variable_assumption( Metamath_database &db, Tokenizer &tokenizer,
  const std::string &label )
{
  if( tokenizer.get_token() != "$f" )
    throw( std::runtime_error("variable assumption does not start with \"$f\"")
      );
  Variable_assumption variable_assumption( label );
  while( tokenizer.peek() != "$." )
    variable_assumption.push_back( db.get_symbol_by_label( tokenizer.get_token()
      ) );
  db.add_variable_assumption( variable_assumption );
  tokenizer.get_token(); // consume "$."
}
//------------------------------------------------------------------------------
void read_assumption( Metamath_database &db, Tokenizer &tokenizer, const
  std::string &label )
{
  if( tokenizer.get_token() != "$e" )
    throw( std::runtime_error("assumption does not start with \"$e\"") );
  Assumption assumption( label );
  while( tokenizer.peek() != "$." )
    assumption.push_back( db.get_symbol_by_label( tokenizer.get_token() ) );
  db.add_assumption( assumption );
  tokenizer.get_token(); // consume "$."
}
//------------------------------------------------------------------------------
void read_theorem( Metamath_database &db, Tokenizer &tokenizer, const
  std::string &label )
{
  if( tokenizer.get_token() != "$p" )
    throw( std::runtime_error("theorem does not start with \"$e\"") );
  Theorem theorem( label );
  while( tokenizer.peek() != "$=" )
    theorem.push_back( db.get_symbol_by_label( tokenizer.get_token() ) );
  while( tokenizer.peek() != "$." )
  {
    db.getStatementByName( tokenizer.get_token() );
  }
  db.add_theorem( theorem );
  tokenizer.get_token(); // consume "$."
}
//------------------------------------------------------------------------------
void read_comment( Metamath_database &db, Tokenizer &tokenizer, const
  std::string &label )
{
  if( tokenizer.get_token() != "$(" )
    throw( std::runtime_error("comment does not start with \"$(\"") );
  while( tokenizer.peek() != "$)" )
    tokenizer.get_token();
  tokenizer.get_token(); // consume "$."
}
//------------------------------------------------------------------------------
void read_dollar_statement( Metamath_database &db, Tokenizer &tokenizer ) {
  std::string label;
  if( tokenizer.peek().at(0) != '$' )
    label = tokenizer.get_token();
  if( tokenizer.peek() == "$a" )
    read_axiom( db, tokenizer, label );
  else if( tokenizer.peek() == "$v" )
    read_variables( db, tokenizer, label );
  else if( tokenizer.peek() == "${" )
    read_scope( db, tokenizer, label );
  else if( tokenizer.peek() == "$c" )
    read_constants( db, tokenizer, label );
  else if( tokenizer.peek() == "$f" )
    read_variable_assumption( db, tokenizer, label );
  else if( tokenizer.peek() == "$e" )
    read_variable_assumption( db, tokenizer, label );
  else if( tokenizer.peek() == "$p" )
    read_theorem( db, tokenizer, label );
  else if( tokenizer.peek() == "$(" )
    read_comment( db, tokenizer, label );
  else
    throw( std::runtime_error("expected label or dollar statment start") );
}
//------------------------------------------------------------------------------
void read_database_from_file( Metamath_database &db, Tokenizer &tokenizer ) {
  while( !tokenizer.peek().empty() )
    read_dollar_statement( db, tokenizer );
}
