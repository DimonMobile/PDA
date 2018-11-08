#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include "fst.h"

namespace PDA
{
namespace Transducer
{

class Tokenizer
{
public:
    Tokenizer(const std::wstring &source);
private:
    void commitToken();
private:
    std::wstring m_token;
    Fst m_fst;
};


} // namespace Transducer
} // namespace PDA
#endif // TOKENIZER_H
