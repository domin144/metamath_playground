#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include "Named.h"

class Symbol : public Named
{
protected:
    Symbol( const std::string &name );
};
//------------------------------------------------------------------------------
class Variable : public Symbol
{
public:
    Variable( const std::string &name );
};
//------------------------------------------------------------------------------
class Constant : public Symbol
{
public:
    Constant( const std::string &name );
};

#endif // SYMBOL_H
