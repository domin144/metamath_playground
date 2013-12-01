#ifndef METAMATH_DATABASE_H
#define METAMATH_DATABASE_H

#include <iostream>
#include <set>
#include <map>

#include "Statement.h"
#include "Symbol.h"
#include "Scope.h"

class Metamath_database
{
public:
    Metamath_database();
    Scoping_statement *get_top_scope();
private:
    Scoping_statement m_top_scope;
};

#endif // METAMATH_DATABASE_H
