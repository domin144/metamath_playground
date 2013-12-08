#ifndef SCOPE_H
#define SCOPE_H

#include "Statement.h"
#include <map>

class Scoping_statement :
    public Statement
{
public:
    Scoping_statement( Scoping_statement *parrent=0 );
    ~Scoping_statement();
    bool is_top() const;
    Symbol *get_symbol_by_label( const std::string &label );
    Named_statement *get_statement_by_label( const std::string &label );
    void add_statement( Statement *statement );
    Scoping_statement *get_parrent();
    Statement *get_first();
    void welcome( Statement_visitor &visitor ) override
    {
        visitor( this );
    }
private:
    Scoping_statement *m_parrent = 0;
    Statement *m_first = 0;
    Statement *m_last = 0;

    std::map< std::string, Symbol *> m_label_to_symbol;
    std::map< std::string, Named_statement *> m_label_to_statement;
};

#endif // SCOPE_H
