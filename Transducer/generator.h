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
        ReturnType = 0x50,
        StackBase,
        StackVertex
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

    static wchar_t registerSuffix(const Register &source);
    //  ASM functions
    static std::wstring call(const std::wstring &label);
    static std::wstring comment(const std::wstring &source);
    static std::wstring push(const Register &source);
    static std::wstring pop(const Register &dest);
    static std::wstring mov(const Register &source, const Register &destination);
    static std::wstring mov(const std::wstring &source, const Register &destination);
    static std::wstring ret();
    static std::wstring syscall();
private:
    void writeLiterals(std::wostream &stream);
    void writeFunctions(std::wostream &stream);
    bool isMainFunctionExists();
    static std::wstring hash(const std::wstring &source);
private:
    int m_mainFunctionExists; // -1 - undefined, -2 false, >=0 - true,
    const Tokenizer &m_tokenizer;
    const StoreFst &m_storeFst;
};

} // namespace Transducer
} // namespace PDA
#endif // GENERATOR_H
