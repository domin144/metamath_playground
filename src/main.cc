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
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

#include "Metamath_database.h"
#include "Metamath_database_read_write.h"
#include "Tokenizer.h"
#include "verify.h"

int main( int argc, char *argv[] ) try
{
    if( argc != 4 )
        throw( std::runtime_error("usage: convert database.mm proof_name "
            "output_database.mm") );
    const std::string db_file_name( argv[1] );
    const std::string proof_name( argv[2] );
    const std::string output_db_file_name( argv[3] );

    Metamath_database db;
    try
    {
        std::ifstream db_file(db_file_name);
        if(!db_file)
            throw std::runtime_error("failed to open file " + db_file_name);
        Tokenizer tokenizer(db_file);
        read_database_from_file(db, tokenizer);
    }
    catch(std::exception &exception)
    {
        throw std::runtime_error(
                    std::string("failed to read database: ")
                    + exception.what());
    }

    try
    {
        std::ofstream output_db_file(output_db_file_name);
        write_database_to_file(db, output_db_file);
    }
    catch(std::exception &exception)
    {
        throw std::runtime_error(
                    std::string("failed to write database: ")
                    + exception.what());
    }

    try
    {
        verify(db);
        std::cout << "verification succeeded\n";
    }
    catch( const verification_failure &e )
    {
        std::cout << "verification failed: " << e.what() << "\n";
    }
    catch(std::exception &exception)
    {
        throw std::runtime_error(
                    std::string("error while verifying the database: ")
                    + exception.what());
    }

//Proof converted_proof = convert_proof_inference( db, proof_name );

    return 0;
}
catch( std::exception &e )
{
    std::cerr << "std::exception caught: " << e.what() << '\n';
}
catch( ... )
{
    std::cerr << "unknown exception caught";
}
