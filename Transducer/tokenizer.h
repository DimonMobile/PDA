#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>

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
};


} // namespace Transducer
} // namespace PDA
#endif // TOKENIZER_H
