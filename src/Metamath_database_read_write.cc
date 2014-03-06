#include <stdexcept>

#include "Metamath_database_read_write.h"

void read_expression( Expression &expression, Tokenizer &tokenizer,
    const Scoping_statement *scope, const std::string &terminating_token="$." )
{
    while( tokenizer.peek() != terminating_token )
    {
        auto symbol = scope->get_symbol_by_label( tokenizer.get_token() );
        expression.push_back( symbol );
    }
}
//------------------------------------------------------------------------------
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
    while( tokenizer.peek() != "$." )
    {
        auto variable = new Variable( tokenizer.get_token() );
        declaration->get_expression().push_back( variable );
    }
    read_expression( declaration->get_expression(), tokenizer, scope );
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
    while( tokenizer.peek() != "$." )
    {
        auto constant = new Constant( tokenizer.get_token() );
        declaration->get_expression().push_back( constant );
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
    read_expression( hypothesis->get_expression(), tokenizer, scope );
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
    read_expression( hypothesis->get_expression(), tokenizer, scope );
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
    read_expression( axiom->get_expression(), tokenizer, scope );
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
    read_expression( theorem->get_expression(), tokenizer, scope, "$=" );
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
    read_expression( restriction->get_expression(), tokenizer, scope );
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
void write_expression_to_file( const Expression &expression,
    std::ostream &output_stream )
{
    for( auto symbol : expression )
        output_stream << symbol->get_name() << ' ';
}
//------------------------------------------------------------------------------
class Statement_writer : public Const_statement_visitor
{
public:
    Statement_writer( const Scoping_statement *scope,
        std::ostream &output_stream );
    void operator()( const Scoping_statement * ) override;
    void operator()( const Constant_declaration * ) override;
    void operator()( const Variable_declaration * ) override;
    void operator()( const Axiom * ) override;
    void operator()( const Theorem * ) override;
    void operator()( const Essential_hypothesis * ) override;
    void operator()( const Floating_hypothesis * ) override;
    void operator()( const Disjoint_variable_restriction * ) override;
private:
    const Scoping_statement *m_scope;
    std::ostream &m_output_stream;
};
//------------------------------------------------------------------------------
Statement_writer::Statement_writer( const Scoping_statement *scope,
    std::ostream &output_stream ) :
    m_scope( scope ),
    m_output_stream( output_stream )
{ }
//------------------------------------------------------------------------------
void Statement_writer::operator()( const Scoping_statement *inner_scope )
{
    m_output_stream << "${\n";
    {
        auto scoped_statement = inner_scope->get_first();
        while( scoped_statement )
        {
            Statement_writer writer( inner_scope, m_output_stream );
            scoped_statement->accept( writer );
            scoped_statement = scoped_statement->get_next();
        }
    }
    m_output_stream << "$}\n";
}
//------------------------------------------------------------------------------
void Statement_writer::operator()( const Constant_declaration *declaration )
{
    m_output_stream << "$c ";
    write_expression_to_file( declaration->get_expression(), m_output_stream );
    m_output_stream << "$.\n";
}
//------------------------------------------------------------------------------
void Statement_writer::operator()( const Variable_declaration *declaration )
{
    m_output_stream << "$v ";
    write_expression_to_file( declaration->get_expression(), m_output_stream );
    m_output_stream << "$.\n";
}
//------------------------------------------------------------------------------
void Statement_writer::operator()( const Axiom *axiom )
{
    m_output_stream << axiom->get_name() << " $a ";
    write_expression_to_file( axiom->get_expression(), m_output_stream );
    m_output_stream << "$.\n";
}
//------------------------------------------------------------------------------
void Statement_writer::operator()( const Theorem *theorem )
{
    m_output_stream << theorem->get_name() << " $p ";
    write_expression_to_file( theorem->get_expression(), m_output_stream );
    m_output_stream << "$= ";
    for( auto proof_step : theorem->get_proof() )
    {
        m_output_stream << proof_step->get_name() << ' ';
    }
    m_output_stream << "$.\n";
}
//------------------------------------------------------------------------------
void Statement_writer::operator()( const Floating_hypothesis *hypothesis )
{
    m_output_stream << hypothesis->get_name() << " $f ";
    write_expression_to_file( hypothesis->get_expression(), m_output_stream );
    m_output_stream << "$.\n";
}
//------------------------------------------------------------------------------
void Statement_writer::operator()( const Essential_hypothesis *hypothesis )
{
    m_output_stream << hypothesis->get_name() << " $e ";
    write_expression_to_file( hypothesis->get_expression(), m_output_stream );
    m_output_stream << "$.\n";
}
//------------------------------------------------------------------------------
void Statement_writer::operator()( const Disjoint_variable_restriction
    *restriction )
{
    m_output_stream << "$d ";
    write_expression_to_file( restriction->get_expression(), m_output_stream );
    m_output_stream << "$.\n";
}
//------------------------------------------------------------------------------
void write_database_to_file( const Metamath_database &db, std::ostream
    &output_stream )
{
    auto scoped_statement = db.get_top_scope()->get_first();
    scoped_statement = scoped_statement->get_next(); // skip invalid axiom
    while( scoped_statement )
    {
        Statement_writer writer( db.get_top_scope(), output_stream );
        scoped_statement->accept( writer );
        scoped_statement = scoped_statement->get_next();
    }
}
