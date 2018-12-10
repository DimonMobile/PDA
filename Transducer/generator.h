#ifndef GENERATOR_H
#define GENERATOR_H

#include <list>

#include "storefst.h"
#include "tokenizer.h"

namespace PDA
{
namespace Transducer
{

struct Register
{
    bool integer = true;
    enum
    {
        ReturnType = 0x50
    };
    enum class Size
    {
        Full,
        Half,
        Quarter,
        OneEight
    } size = Size::Full;
    unsigned short argumentIndex = ReturnType;
    std::wstring toString() const;
    Register(unsigned short index, Size asize);
};

class Generator
{
public:
    Generator(const Tokenizer &tokenizer, const StoreFst &storeFst);

    static std::wstring mov(const Register &source, const Register &destination);
    static std::wstring mov(const std::wstring &source, const Register &destination);
    static std::wstring syscall();
private:

};

} // namespace Transducer
} // namespace PDA
#endif // GENERATOR_H
