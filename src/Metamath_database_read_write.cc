#include <stdexcept>

#include "Metamath_database_read_write.h"

void read_statement( Scoping_statement *scope, Tokenizer &tokenizer );
//------------------------------------------------------------------------------
void read_scope( Scoping_statement *scope, Tokenizer &tokenizer,
    const std::string &label )
{
    if( tokenizer.get_token() != "${" )
        throw( std::runtime_error("scope does not start with \"${\"") );

    auto new_scope = new Scoping_statement( scope );
    while( tokenizer.peek() != "$}" )
        read_statement( new_scope, tokenizer );
    scope->add_statement( new_scope );
    tokenizer.get_token(); // consume "$}"
}
//------------------------------------------------------------------------------
void read_variables( Scoping_statement *scope, Tokenizer &tokenizer, const
  std::string &label )
{
    if( tokenizer.get_token() != "$v" )
        throw( std::runtime_error("variables do not start with \"$v\"") );

    auto declaration = new Variable_declaration();
    auto &expression = declaration->get_expression();
    while( tokenizer.peek() != "$." )
    {
        auto variable = new Variable( tokenizer.get_token() );
        expression.push_back( variable );
    }
    scope->add_statement( declaration );
    tokenizer.get_token(); // consume "$."
}
//------------------------------------------------------------------------------
void read_constants( Scoping_statement *scope, Tokenizer &tokenizer, const
  std::string &label )
{
    if( tokenizer.get_token() != "$c" )
        throw( std::runtime_error("constants do not start with \"$c\"") );

    auto declaration = new Constant_declaration();
    auto &expression = declaration->get_expression();
    while( tokenizer.peek() != "$." )
    {
        auto constant = new Constant( tokenizer.get_token() );
        expression.push_back( constant );
    }
    scope->add_statement( declaration );
    tokenizer.get_token(); // consume "$."
}
//------------------------------------------------------------------------------
void read_floating_hypothesis( Scoping_statement *scope, Tokenizer &tokenizer,
    const std::string &label )
{
    if( tokenizer.get_token() != "$f" )
        throw( std::runtime_error("variable assumption does not start with "
            "\"$f\"") );

    auto hypothesis = new Floating_hypothesis( label );
    auto &expression = hypothesis->get_expression();
    while( tokenizer.peek() != "$." )
    {
        auto symbol = scope->get_symbol_by_label( tokenizer.get_token() );
        expression.push_back( symbol );
    }
    scope->add_statement( hypothesis );
    tokenizer.get_token(); // consume "$."
}
//------------------------------------------------------------------------------
void read_essential_hypothesis( Scoping_statement *scope, Tokenizer &tokenizer, const
    std::string &label )
{
    if( tokenizer.get_token() != "$e" )
        throw( std::runtime_error("assumption does not start with \"$e\"") );

    auto hypothesis = new Essential_hypothesis( label );
    auto &expression = hypothesis->get_expression();
    while( tokenizer.peek() != "$." )
    {
        auto symbol = scope->get_symbol_by_label( tokenizer.get_token() );
        expression.push_back( symbol );
    }
    scope->add_statement( hypothesis );
    tokenizer.get_token(); // consume "$."
}
//------------------------------------------------------------------------------
void read_axiom( Scoping_statement *scope, Tokenizer &tokenizer,
    const std::string &label )
{
    if( tokenizer.get_token() != "$a" )
        throw( std::runtime_error("axiom does not start with \"$a\"") );

    auto axiom = new Axiom( label );
    auto &expression = axiom->get_expression();
    while( tokenizer.peek() != "$." )
    {
        auto symbol = scope->get_symbol_by_label( tokenizer.get_token() );
        expression.push_back( symbol );
    }
    scope->add_statement( axiom );
    tokenizer.get_token(); // consume "$."
}
//------------------------------------------------------------------------------
void read_theorem( Scoping_statement *scope, Tokenizer &tokenizer, const
    std::string &label )
{
    if( tokenizer.get_token() != "$p" )
        throw( std::runtime_error("theorem does not start with \"$e\"") );

    auto theorem = new Theorem( label );
    auto &expression = theorem->get_expression();
    while( tokenizer.peek() != "$=" )
    {
        auto symbol = scope->get_symbol_by_label( tokenizer.get_token() );
        expression.push_back( symbol );
    }
    tokenizer.get_token(); // consume "$="
    while( tokenizer.peek() != "$." )
    {
        auto statement = scope->get_statement_by_label( tokenizer.get_token() );
        theorem->get_proof().push_back( statement );
    }
    scope->add_statement( theorem );
    tokenizer.get_token(); // consume "$."
}
//------------------------------------------------------------------------------
void read_disjoint_variable_restriction( Scoping_statement *scope,
    Tokenizer &tokenizer, const std::string &label )
{
    if( tokenizer.get_token() != "$d" )
        throw( std::runtime_error("disjoint variable restriction does not start"
            " with \"$v\"") );

    auto restriction = new Disjoint_variable_restriction();
    auto &expression = restriction->get_expression();
    while( tokenizer.peek() != "$." )
    {
        auto variable = new Variable( tokenizer.get_token() );
        expression.push_back( variable );
    }
    scope->add_statement( restriction );
    tokenizer.get_token(); // consume "$."
}
//------------------------------------------------------------------------------
void read_comment( Scoping_statement *scope, Tokenizer &tokenizer, const
  std::string &label )
{
    if( tokenizer.get_token() != "$(" )
        throw( std::runtime_error("comment does not start with \"$(\"") );

    while( tokenizer.peek() != "$)" )
        tokenizer.get_token();
    tokenizer.get_token(); // consume "$)"
}
//------------------------------------------------------------------------------
void read_statement( Scoping_statement *scope, Tokenizer &tokenizer )
{
    std::string label;
    if( tokenizer.peek().at(0) != '$' )
        label = tokenizer.get_token();

    if( tokenizer.peek() == "$a" )
    {
        read_axiom( scope, tokenizer, label );
    }
    else if( tokenizer.peek() == "$v" )
    {
        read_variables( scope, tokenizer, label );
    }
    else if( tokenizer.peek() == "${" )
    {
        read_scope( scope, tokenizer, label );
    }
    else if( tokenizer.peek() == "$c" )
    {
        read_constants( scope, tokenizer, label );
    }
    else if( tokenizer.peek() == "$f" )
    {
        read_floating_hypothesis( scope, tokenizer, label );
    }
    else if( tokenizer.peek() == "$e" )
    {
        read_essential_hypothesis( scope, tokenizer, label );
    }
    else if( tokenizer.peek() == "$p" )
    {
        read_theorem( scope, tokenizer, label );
    }
    else if( tokenizer.peek() == "$d" )
    {
        read_disjoint_variable_restriction( scope, tokenizer, label );
    }
    else if( tokenizer.peek() == "$(" )
    {
        read_comment( scope, tokenizer, label );
    }
    else
    {
        throw( std::runtime_error("expected label or dollar statment start") );
    }
}
//------------------------------------------------------------------------------
void read_database_from_file( Metamath_database &db, Tokenizer &tokenizer )
{
    while( !tokenizer.peek().empty() )
    {
        read_statement( db.get_top_scope(), tokenizer );
    }
}
//------------------------------------------------------------------------------
void write_expression_to_file( const std::vector<Symbol *> &expression,
    std::ostream &output_stream )
{
    for( auto symbol : expression )
        output_stream << symbol->get_name() << ' ';
}
//------------------------------------------------------------------------------
void write_statement_to_file( Scoping_statement *scope, Statement *statement,
    std::ostream &output_stream )
{
    switch( statement->get_type() )
    {
    case Statement::Type::axiom:
        output_stream <<
            static_cast<Named_statement *>( statement )->get_name() << ' ';
        output_stream << "$a ";
        write_expression_to_file( statement->get_expression(), output_stream );
        output_stream << "$.\n";
        break;
    case Statement::Type::theorem:
        output_stream <<
            static_cast<Named_statement *>( statement )->get_name() << ' ';
        output_stream << "$p ";
        write_expression_to_file( statement->get_expression(), output_stream );
        output_stream << "$= ";
        for( auto proof_step :
            static_cast<Theorem *>( statement )->get_proof() )
        {
            output_stream << proof_step->get_name() << ' ';
        }
        output_stream << "$.\n";
        break;
    case Statement::Type::constant_declaration:
        output_stream << "$c ";
        write_expression_to_file( statement->get_expression(), output_stream );
        output_stream << "$.\n";
        break;
    case Statement::Type::variable_declaration:
        output_stream << "$v ";
        write_expression_to_file( statement->get_expression(), output_stream );
        output_stream << "$.\n";
        break;
    case Statement::Type::floating_hypothesis:
        output_stream <<
            static_cast<Named_statement *>( statement )->get_name() << ' ';
        output_stream << "$f ";
        write_expression_to_file( statement->get_expression(), output_stream );
        output_stream << "$.\n";
        break;
    case Statement::Type::essential_hypothesis:
        output_stream <<
            static_cast<Named_statement *>( statement )->get_name() << ' ';
        output_stream << "$e ";
        write_expression_to_file( statement->get_expression(), output_stream );
        output_stream << "$.\n";
        break;
    case Statement::Type::disjoint_variable_restriction:
        output_stream << "$d ";
        write_expression_to_file( statement->get_expression(), output_stream );
        output_stream << "$.\n";
        break;
    case Statement::Type::scoping_statement:
        output_stream << "${\n";
        {
            auto inner_scope = static_cast<Scoping_statement *>( statement );
            auto scoped_statement =
                inner_scope->get_first();
            while( scoped_statement )
            {
                write_statement_to_file( inner_scope, scoped_statement,
                    output_stream );
                scoped_statement = scoped_statement->get_next();
            }
        }
        output_stream << "$}\n";
        break;
    default:
        throw( std::runtime_error( "invalid statement type" ) );
    }
}
//------------------------------------------------------------------------------
void write_database_to_file( Metamath_database &db, std::ostream
    &output_stream )
{
    auto scoped_statement = db.get_top_scope()->get_first();
    scoped_statement = scoped_statement->get_next(); // skip invalid axiom
    while( scoped_statement )
    {
        write_statement_to_file( db.get_top_scope(), scoped_statement,
            output_stream );
        scoped_statement = scoped_statement->get_next();
    }
}
