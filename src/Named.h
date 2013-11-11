#ifndef NAMED_H
#define NAMED_H

#include <string>

class Named {
public:
  Named( const std::string &name );
  const std::string &get_name() const;
private:
  std::string m_name;
};
//------------------------------------------------------------------------------
class Less_by_name {
public:
  bool operator()( const Named &lhs, const Named &rhs ) const;
};

#endif // NAMED_H
