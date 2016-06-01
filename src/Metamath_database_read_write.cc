/* 
 * Copyright 2013 Dominik Wójt
 * 
 * This file is part of metamath_playground.
 * 
 * metamath_playground is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * metamath_playground is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with metamath_playground.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <stdexcept>
#include <utility>

#include "Metamath_database_read_write.h"
#include "Statement_visitors.h"
#include "Proof_step.h"
#include "verify.h"

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
    scope->add_statement( new_scope );
    while( tokenizer.peek() != "$}" )
        read_statement( new_scope, tokenizer );
    tokenizer.get_token(); // consume "$}"
}
//------------------------------------------------------------------------------
void read_variables( Scoping_statement *scope, Tokenizer &tokenizer, const
  std::string &label )
{
    if( tokenizer.get_token() != "$v" )
        throw( std::runtime_error("variables do not start with \"$v\"") );

    auto declaration = new Variable_declaration();
    scope->add_statement( declaration );
    while( tokenizer.peek() != "$." )
    {
        auto variable = new Variable( tokenizer.get_token() );
        declaration->get_expression().push_back( variable );
        scope->register_symbol( variable );
    }
    tokenizer.get_token(); // consume "$."
}
//------------------------------------------------------------------------------
void read_constants( Scoping_statement *scope, Tokenizer &tokenizer, const
  std::string &label )
{
    if( tokenizer.get_token() != "$c" )
        throw( std::runtime_error("constants do not start with \"$c\"") );
    if( !scope->is_top() )
        throw std::runtime_error( "constant declaration not in top scope" );

    auto declaration = new Constant_declaration();
    scope->add_statement( declaration );
    while( tokenizer.peek() != "$." )
    {
        auto constant = new Constant( tokenizer.get_token() );
        declaration->get_expression().push_back( constant );
        scope->register_symbol( constant );
    }
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
    scope->add_statement( hypothesis );
    read_expression( hypothesis->get_expression(), tokenizer, scope );
    tokenizer.get_token(); // consume "$."
}
//------------------------------------------------------------------------------
void read_essential_hypothesis( Scoping_statement *scope, Tokenizer &tokenizer,
    const std::string &label )
{
    if( tokenizer.get_token() != "$e" )
        throw( std::runtime_error("assumption does not start with \"$e\"") );

    auto hypothesis = new Essential_hypothesis( label );
    scope->add_statement( hypothesis );
    read_expression( hypothesis->get_expression(), tokenizer, scope );
    tokenizer.get_token(); // consume "$."
}
//------------------------------------------------------------------------------
void read_axiom( Scoping_statement *scope, Tokenizer &tokenizer,
    const std::string &label )
{
    if( tokenizer.get_token() != "$a" )
        throw( std::runtime_error("axiom does not start with \"$a\"") );

    auto axiom = new Axiom( label );
    scope->add_statement( axiom );
    read_expression( axiom->get_expression(), tokenizer, scope );
    tokenizer.get_token(); // consume "$."
}
//------------------------------------------------------------------------------
class Compressed_proof_code_extractor
{
public:
    Compressed_proof_code_extractor( Tokenizer &tokenizer ) :
        m_tokenizer( tokenizer )
    { }
    int extract_number()
    {
        int n = 0;
        while( true )
        {
            char z = get_character();
            if( 'A'<=z && z<='T' )
            {
                n = n*20 + z-'A' + 1;
                return n;
            }
            else if( 'U'<=z && z<='Y' )
            {
                n = n*5 + z-'U' + 1;
            }
            else if( z=='Z' )
            {
                throw std::runtime_error( "error: Z found in compressed "
                    "proof when number incomplete" );
            }
            else
            {
                throw std::runtime_error( "error: invalid character found in "
                    "compressed proof" );
            }
        }
    }
    bool extract_reference_flag()
    {
        if( peek_character() == 'Z' )
        {
            get_character();
            return true;
        }
        else
        {
            return false;
        }
    }
    bool is_end_of_proof()
    {
        return peek_character() == 0;
    }

private:
    void fill_buffer()
    {
        if( m_tokenizer.peek() != "$." )
            m_buffer += m_tokenizer.get_token();
    }
    char peek_character()
    {
        if( m_buffer.empty() )
            fill_buffer();
        if( m_buffer.empty() )
            return 0; // end of compressed sequence
        else
            return m_buffer.front();
    }
    char get_character()
    {
        char result = peek_character();
        if( result==0 )
            throw std::runtime_error( "read got past end of compressed "
                "sequence" );
        m_buffer.erase( m_buffer.begin() );
        return result;
    }

    Tokenizer &m_tokenizer;
    std::string m_buffer;
};
//------------------------------------------------------------------------------
class Proof_collector : private Lazy_const_statement_visitor
{
public:
    Proof_step *new_step( const Statement *statement )
    {
        statement->accept( *this );
        if( !m_step )
            throw std::runtime_error( "invalid proof step read" );
        return m_step;
    }
private:
    Proof_step *m_step = 0;
    void operator()( const Floating_hypothesis *hypothesis ) override
    {
        m_step = new Floating_hypothesis_step( hypothesis );
    }
    void operator()( const Essential_hypothesis *hypothesis ) override
    {
        m_step = new Essential_hypothesis_step( hypothesis );
    }
    void operator()( const Theorem *theorem ) override
    {
        new_assertion_step( theorem );
    }
    void operator()( const Axiom *axiom ) override
    {
        new_assertion_step( axiom );
    }
    void new_assertion_step( const Assertion *assertion )
    {
        m_step = new Assertion_step( assertion );
    }
};
//------------------------------------------------------------------------------
void read_compressed_proof( Scoping_statement *scope, Tokenizer &tokenizer,
    Proof &proof, const Theorem *theorem )
{
    proof.set_compressed( true );
    tokenizer.get_token(); // read "("

    std::vector<const Named_statement *> mandatory_hypotheses;
    {
        Frame frame( theorem );
        collect_frame( frame );
        std::swap( mandatory_hypotheses, frame.get_statements() );
        mandatory_hypotheses.pop_back();
    }
    const int mandatory_hypotheses_count = mandatory_hypotheses.size();

    // read referred statements
    std::vector<const Named_statement *> referred_statements;
    while( tokenizer.peek() != ")" )
    {
        auto statement = scope->get_statement_by_label( tokenizer.get_token() );
        referred_statements.push_back( statement );
    }
    const int referred_statements_count = referred_statements.size();
    tokenizer.get_token(); // read ")"

    int referred_expressions_count = 0;
    Compressed_proof_code_extractor extractor( tokenizer );
    Proof_collector proof_collector;
    while( !extractor.is_end_of_proof() )
    {
        int number = extractor.extract_number()-1;
        if( number < mandatory_hypotheses_count )
        {
            proof.get_steps().push_back( proof_collector.new_step(
                mandatory_hypotheses[number] ) );
        }
        else if( ( number -= mandatory_hypotheses_count ) <
            referred_statements_count )
        {
            proof.get_steps().push_back( proof_collector.new_step(
                referred_statements[number] ) );
        }
        else if( ( number -= referred_statements_count ) <
            referred_expressions_count )
        {
            proof.get_steps().push_back( new Refer_step( number ) );
        }
        else
        {
            throw std::runtime_error(
                "invalid number read in compressed proof" );
        }
        if( extractor.extract_reference_flag() )
        {
            proof.get_steps().push_back( new Add_reference_step() );
            referred_expressions_count++;
        }
    }
}
//------------------------------------------------------------------------------
void read_uncompressed_proof( Scoping_statement *scope, Tokenizer &tokenizer,
    Proof &proof )
{
    proof.set_compressed( false );
    while( tokenizer.peek() != "$." )
    {
        auto name = tokenizer.get_token();
        if( name == "?" )
        {
            proof.get_steps().push_back( new Unknown_step() );
        }
        else
        {
            auto statement = scope->get_statement_by_label( name );
            proof.get_steps().push_back(
                Proof_collector().new_step( statement ) );
        }
    }
}
//------------------------------------------------------------------------------
void read_theorem( Scoping_statement *scope, Tokenizer &tokenizer, const
    std::string &label )
{
    if( tokenizer.get_token() != "$p" )
        throw( std::runtime_error("theorem does not start with \"$e\"") );

    auto theorem = new Theorem( label );
    scope->add_statement( theorem );
    read_expression( theorem->get_expression(), tokenizer, scope, "$=" );
    tokenizer.get_token(); // consume "$="
    if( tokenizer.peek() == "(" )
    {
        read_compressed_proof( scope, tokenizer, theorem->get_proof(),
            theorem );
    }
    else
    {
        read_uncompressed_proof( scope, tokenizer, theorem->get_proof() );
    }
    tokenizer.get_token(); // consume "$."

    verify( theorem );
}
//------------------------------------------------------------------------------
void read_disjoint_variable_restriction( Scoping_statement *scope,
    Tokenizer &tokenizer, const std::string &label )
{
    if( tokenizer.get_token() != "$d" )
        throw( std::runtime_error("disjoint variable restriction does not start"
            " with \"$v\"") );

    auto restriction = new Disjoint_variable_restriction();
    scope->add_statement( restriction );
    read_expression( restriction->get_expression(), tokenizer, scope );
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
    Statement_writer( std::ostream &output_stream );
    void operator()( const Scoping_statement * ) override;
    void operator()( const Constant_declaration * ) override;
    void operator()( const Variable_declaration * ) override;
    void operator()( const Axiom * ) override;
    void operator()( const Theorem * ) override;
    void operator()( const Essential_hypothesis * ) override;
    void operator()( const Floating_hypothesis * ) override;
    void operator()( const Disjoint_variable_restriction * ) override;
private:
    std::ostream &m_output_stream;
};
//------------------------------------------------------------------------------
Statement_writer::Statement_writer( std::ostream &output_stream ) :
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
            Statement_writer writer( m_output_stream );
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
class Proof_writer : protected Const_proof_step_visitor
{
public:
    Proof_writer( std::ostream &output_steram ) :
        m_output_stream( output_steram )
    { }
    void write( const Proof &proof )
    {
        for( auto step : proof.get_steps() )
        {
            step->accept( *this );
        }
    }

protected:
    std::ostream &m_output_stream;
};
//------------------------------------------------------------------------------
class Compressed_proof_writer : public Proof_writer
{
public:
    typedef std::map<const Statement *, int> Map_type;
    Compressed_proof_writer( std::ostream &output_stream,
        const Map_type &map
    ) :
        Proof_writer( output_stream ),
        m_statement_to_number_map( map )
    { }

private:
    void operator()( const Assertion_step *step ) override
    {
        push_statement( step->get_assertion() );
    }
    void operator()( const Essential_hypothesis_step *step ) override
    {
        push_statement( step->get_hypothesis() );
    }
    void operator()( const Floating_hypothesis_step *step ) override
    {
        push_statement( step->get_hypothesis() );
    }
    void operator()( const Add_reference_step * ) override
    {
        m_output_stream << 'Z';
    }
    void operator()( const Refer_step *step ) override
    {
        const int referred_statements_count = m_statement_to_number_map.size();
        encode_number( step->get_index() + referred_statements_count + 1 );
    }
    void operator()( const Unknown_step *step ) override
    {
        m_output_stream << '?';
    }
    void push_statement( const Statement *statement )
    {
        const auto pair = m_statement_to_number_map.find( statement );
        if( pair == m_statement_to_number_map.end() )
        {
            throw std::runtime_error( "statement outside of the reference list "
                "found when writting compressed proof" );
        }
        encode_number( pair->second );
    }
    void encode_number( int number )
    {
        number--;
        if( number < 0 )
        {
            throw std::runtime_error( "n < 1" );
        }

        std::string output;
        output.insert( 0, 1, 'A'+number%20 );
        number /= 20;
        while( number > 0 )
        {
            number--;
            output.insert( 0, 1, 'U'+number%5 );
            number /= 5;
        }
        m_output_stream << output;
    }

    const Map_type &m_statement_to_number_map;
};
//------------------------------------------------------------------------------
class Compressed_proof_reference_collector : public Const_proof_step_visitor
{
public:
    typedef Compressed_proof_writer::Map_type Map_type;
    typedef std::vector<const Named_statement *> Vector_type;
    Compressed_proof_reference_collector( Map_type &map,
        Vector_type &additional_statements
    ) :
        m_statement_to_number_map( map ),
        m_additional_statements( additional_statements )
    { }
    void collect( const Proof &proof, const Theorem *theorem )
    {
        std::vector<const Named_statement *> mandatory_hypotheses;
        {
            Frame frame( theorem );
            collect_frame( frame );
            std::swap( mandatory_hypotheses, frame.get_statements() );
            mandatory_hypotheses.pop_back();
        }
        for( auto hypothesis : mandatory_hypotheses )
        {
            collect_to_map( hypothesis );
        }

        for( auto step : proof.get_steps() )
        {
            step->accept( *this );
        }
    }

private:
    void operator()( const Assertion_step *step ) override
    {
        collect_to_vector( step->get_assertion() );
        collect_to_map( step->get_assertion() );
    }
    void operator()( const Essential_hypothesis_step *step ) override
    {
        collect_to_vector( step->get_hypothesis() );
        collect_to_map( step->get_hypothesis() );
    }
    void operator()( const Floating_hypothesis_step *step ) override
    {
        collect_to_vector( step->get_hypothesis() );
        collect_to_map( step->get_hypothesis() );
    }
    void operator()( const Add_reference_step * ) override
    { }
    void operator()( const Refer_step * ) override
    { }
    void operator()( const Unknown_step * )
    { }
    void collect_to_map( const Named_statement *statement )
    {
        if( m_statement_to_number_map.find( statement ) ==
            m_statement_to_number_map.end() )
        {
            const int n = m_statement_to_number_map.size();
            m_statement_to_number_map.insert( std::make_pair( statement, n ) );
        }
    }
    void collect_to_vector( const Named_statement *statement )
    {
        if( m_statement_to_number_map.find( statement ) ==
            m_statement_to_number_map.end() )
        {
            m_additional_statements.push_back( statement );
        }
    }

    Map_type &m_statement_to_number_map;
    Vector_type &m_additional_statements;
};
//------------------------------------------------------------------------------
class Uncompressed_proof_writer : public Proof_writer
{
public:
    Uncompressed_proof_writer( std::ostream &output_stream ) :
        Proof_writer( output_stream )
    { }

private:
    void operator()( const Assertion_step *step ) override
    {
        write_named_statement( step->get_assertion() );
    }
    void operator()( const Essential_hypothesis_step *step ) override
    {
        write_named_statement( step->get_hypothesis() );
    }
    void operator()( const Floating_hypothesis_step *step ) override
    {
        write_named_statement( step->get_hypothesis() );
    }
    void operator()( const Add_reference_step * ) override
    {
        throw_compressed_step();
    }
    void operator()( const Refer_step * ) override
    {
        throw_compressed_step();
    }
    void operator()( const Unknown_step * )
    {
        m_output_stream << "? ";
    }
    void write_named_statement( const Named_statement *statement )
    {
        m_output_stream << statement->get_name() << ' ';
    }
    void throw_compressed_step()
    {
        throw std::runtime_error( "compressed step found in uncompressed proof,"
            " decompression on the fly not implemented yet" );
    }
};
//------------------------------------------------------------------------------
void Statement_writer::operator()( const Theorem *theorem )
{
    m_output_stream << theorem->get_name() << " $p ";
    write_expression_to_file( theorem->get_expression(), m_output_stream );
    m_output_stream << "$= ";
    if( theorem->get_proof().get_compressed() )
    {
        Compressed_proof_reference_collector::Map_type map;
        Compressed_proof_reference_collector::Vector_type additional_sentences;
        Compressed_proof_reference_collector collector( map,
            additional_sentences );
        collector.collect( theorem->get_proof(), theorem );

        // list additional statements
        m_output_stream << "( ";
        for( auto sentence : additional_sentences )
        {
            m_output_stream << sentence->get_name() << " ";
        }
        m_output_stream << ") ";

        Compressed_proof_writer writer( m_output_stream, map );
        writer.write( theorem->get_proof() );
    }
    else
    {
        Uncompressed_proof_writer writer( m_output_stream );
        writer.write( theorem->get_proof() );
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
        Statement_writer writer( output_stream );
        scoped_statement->accept( writer );
        scoped_statement = scoped_statement->get_next();
    }
}
