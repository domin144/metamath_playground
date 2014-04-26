#ifndef VERIFY_H
#define VERIFY_H

#include "Metamath_database.h"

class Frame
{
public:
    Frame( const Assertion *assertion )
    {
        m_statements.push_back( assertion );
    }
    std::vector<const Named_statement *> &get_statements()
    {
        return m_statements;
    }
    const std::vector<const Named_statement *> &get_statements() const
    {
        return m_statements;
    }
    std::vector<const Disjoint_variable_restriction *> &get_restrictions()
    {
        return m_restrictions;
    }
    const std::vector<const Disjoint_variable_restriction *> &get_restrictions()
        const
    {
        return m_restrictions;
    }

private:
    std::vector<const Named_statement *> m_statements;
    std::vector<const Disjoint_variable_restriction *> m_restrictions;
};
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
void collect_extended_frame( Frame &frame );
void collect_frame( Frame &frame );
void verify( const Theorem *theorem );
void verify( const Metamath_database &db );

#endif // VERIFY_H
