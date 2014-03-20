#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <iostream>

class Tokenizer {
public:
    Tokenizer( std::istream &input_stream );
    std::string get_token();
    const std::string &peek();
private:
    void extract_next_token();
    std::istream &m_input_stream;
    std::string m_next_token;
};

#endif // TOKENIZER_H
