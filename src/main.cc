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
