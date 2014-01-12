#ifndef VERIFY_H
#define VERIFY_H

#include "Metamath_database.h"

bool verify( const Theorem *theorem );
bool verify( const Metamath_database &db );

#endif // VERIFY_H
