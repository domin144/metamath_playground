#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>

class Symbol {
protected:
  Symbol( const std::string &name );
public:
  const std::string &get_name() const;
  bool operator<( const Symbol &other_symbol ) const;
private:
  std::string m_name;
};
//------------------------------------------------------------------------------
class Variable : public Symbol {
public:
  Variable( const std::string &name );
};
//------------------------------------------------------------------------------
class Constant : public Symbol {
public:
  Constant( const std::string &name );
};

#endif // SYMBOL_H
