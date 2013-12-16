#ifndef METAMATH_DATABASE_READ_H
#define METAMATH_DATABASE_READ_H

#include <iostream>

#include "Tokenizer.h"
#include "Metamath_database.h"

void read_database_from_file( Metamath_database &db, Tokenizer &tokenizer );
void write_database_to_file( const Metamath_database &db, std::ostream
  &output_stream );

#endif // METAMATH_DATABASE_READ_H
