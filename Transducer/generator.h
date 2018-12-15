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
        StackTop
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
    Register(unsigned short index, Size assize, int offset);
    bool isPtr = false;
    int ptrOffset = 0;
};

class Generator
{
public:
    Generator(const Tokenizer &tokenizer, const StoreFst &storeFst);

    static wchar_t registerSuffix(const Register &source);
    //  ASM functions
    static std::wstring add(const int source, const Register &dest, const wchar_t suff = L'\0');
    static std::wstring sub(const int source, const Register &dest, const wchar_t suff = L'\0');
    static std::wstring sub(const Register &source, const int dest, const wchar_t suff = L'\0');
    static std::wstring sub(const Register &source, const std::wstring &dest, const wchar_t suff = L'\0');
    static std::wstring call(const std::wstring &label);
    static std::wstring comment(const std::wstring &source);
    static std::wstring push(const Register &source, const wchar_t suff = L'\0');
    static std::wstring pop(const Register &dest, const wchar_t suff = L'\0');
    static std::wstring mov(const Register &source, const Register &dest, const wchar_t suff = L'\0');
    static std::wstring mov(const std::wstring &source, const Register &dest, const wchar_t suff = L'\0');
    static std::wstring mov(const int source, const Register &dest, const wchar_t suff = L'\0');
    static std::wstring ret();
    static std::wstring syscall();
private:
    void writeLiterals(std::wostream &stream);
    void writeFunctions(std::wostream &stream);
    void writeFunctionBody(std::wostream &stream, const size_t startTokenIndex);
    void writeAsembledExpression(std::wostream &stream, const std::vector<Token> &operation);
    void writeAssembledOperation(std::wostream &stream, const std::vector<Token> &operation);
    void writeGlobalFunctions(std::wostream &stream);
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
