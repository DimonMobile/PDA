#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <list>
#include "fst.h"

namespace PDA
{
namespace Transducer
{

struct Token
{
    wchar_t token;
    int position, line;
};

class Tokenizer
{
public:
    Tokenizer(const std::wstring &source);
private:
    void commitToken();
private:
    int m_currentPos, m_currentLine;
    std::wstring m_currentFile;
    std::wstring m_token;
    Fst m_fst;
};


} // namespace Transducer
} // namespace PDA
#endif // TOKENIZER_H
