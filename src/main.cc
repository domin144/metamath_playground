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

    try
    {
        std::ifstream db_file;
        db_file.exceptions( std::ios_base::badbit | std::ios_base::failbit |
                            std::ios_base::eofbit );
        db_file.open( db_file_name );
        Tokenizer tokenizer( db_file );
        Metamath_database db;
        read_database_from_file( db, tokenizer );

        std::ofstream output_db_file( output_db_file_name );
        write_database_to_file( db, output_db_file );

    //Proof converted_proof = convert_proof_inference( db, proof_name );

        verify( db );
        std::cout << "verification succeeded\n";
    }
    catch( const verification_failure &e )
    {
        std::cout << "verification failed: " << e.what() << "\n";
    }

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
