#ifndef VERIFY_H
#define VERIFY_H

#include "Metamath_database.h"

class verification_failure
{
public:
    verification_failure( const std::string &what ) :
        m_what( what )
    { }
    const std::string &what() const
    {
        return m_what;
    }
private:
    const std::string m_what;
};
void verify( const Theorem *theorem );
void verify( const Metamath_database &db );

#endif // VERIFY_H
