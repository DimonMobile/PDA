#include "generator.h"

#include <string>
#include <fstream>

#include "Utils/misc.h"
#include "Transducer/expressiontoken.h"

#define UNUSED(x) (void)x

namespace PDA
{
namespace Transducer
{

namespace Constants
{
    const std::wstring globlStartLabel = L".globl _start";
    const std::wstring startLabel = L"_start:";
    const std::wstring firstLineComment = L"# PDA v1.0 Complier; LGPL3 https://www.gnu.org/licenses/lgpl-3.0.en.html ";
    const std::wstring sectionDataString = L".section .data";
    const std::wstring sectionUninitializedDataString = L".section .bss";
    const std::wstring sectionTextString = L".section .text";
}

Generator::Generator(const Tokenizer &tokenizer, const StoreFst &storeFst) : m_mainFunctionExists(-1), m_tokenizer(tokenizer), m_storeFst(storeFst)
{
    UNUSED(tokenizer);
    UNUSED(storeFst);

    std::wofstream ostream("result.s");
    ostream << Constants::firstLineComment << std::endl;
    ostream << Constants::sectionDataString << std::endl;
    writeLiterals(ostream);
    ostream << Constants::sectionUninitializedDataString << std::endl;
    ostream << Constants::sectionTextString << std::endl;
    if (isMainFunctionExists())
    {
        ostream << Constants::globlStartLabel << std::endl;
        ostream << Constants::startLabel << std::endl;
        ostream << call(  hash(m_tokenizer.identifiers()[static_cast<size_t>(m_mainFunctionExists)].decoratedName) ) << std::endl;
        ostream << mov(60, Register(Register::ReturnType, Register::Size::Full)) << std::endl;
        ostream << mov(0, Register(0, Register::Size::Full)) << std::endl;
        ostream << syscall() << std::endl;
    }
    else
    {
        ostream << comment(L"Main function not found, then all functions are extern") << std::endl;
        writeGlobalFunctions(ostream);
    }
    writeFunctions(ostream);
}

wchar_t Generator::registerSuffix(const Register &source)
{
    if (source.integer)
    {
        switch(source.size)
        {
        case Register::Size::Full:
            return L'q';
        case Register::Size::Half:
            return L'l';
        case Register::Size::Quarter:
            return L'w';
        case Register::Size::OneEight:
            return L'b';
        }
    }
    return L'q';
}

std::wstring Generator::sub(const int source, const Register &dest)
{
    std::wstring result = L"sub";
    result += registerSuffix(dest);
    result += L'\t';
    result += L'$' + std::to_wstring(source);
    result += L",\t%" + dest.toString();

    return result;
}

std::wstring Generator::sub(const Register &source, const int dest)
{
    std::wstring result = L"sub";
    result += registerSuffix(source);
    result += L"\t";
    result += L'%' + source.toString();
    result += L",\t";
    result += L"$" + std::to_wstring(dest);
    return result;
}

std::wstring Generator::sub(const Register &source, const std::wstring &dest)
{
    std::wstring result = L"sub";
    result += registerSuffix(source);
    result += L"\t";
    result += L'%' + source.toString();
    result += L",\t";
    result += dest;
    return result;
}

std::wstring Generator::call(const std::wstring &label)
{
    std::wstring result = L"call\t";
    result += label;
    return result;
}

std::wstring Generator::comment(const std::wstring &source)
{
    std::wstring result;
    result = L"\t#***\t" + source + L"\t***#";
    return result;
}

std::wstring Generator::push(const Register &source)
{
    std::wstring result = L"push";
    result += registerSuffix(source);
    result += L'\t';
    result += L'%' + source.toString();
    return result;
}

std::wstring Generator::pop(const Register &dest)
{
    std::wstring result = L"pop";
    result += registerSuffix(dest);
    result += L'\t';
    result += L'%' + dest.toString();
    return result;
}

std::wstring Generator::mov(const Register &source, const Register &destination)
{
    std::wstring result = L"mov";
    result += registerSuffix(source);
    result += L"\t%";
    result += source.toString();
    result += L",\t%";
    result += destination.toString();
    return result;
}

std::wstring Generator::mov(const std::wstring &source, const Register &destination)
{
    std::wstring result = L"mov";
    result += registerSuffix(destination);
    result += L"\t" + source;
    result += L",\t%" + destination.toString();
    return result;
}

std::wstring Generator::mov(const int source, const Register &destination)
{
    std::wstring result = L"mov";
    result += registerSuffix(destination);
    result += L"\t$" + std::to_wstring(source);
    result += L",\t%" + destination.toString();
    return result;
}

std::wstring Generator::ret()
{
    return L"ret";
}

std::wstring Generator::syscall()
{
    return L"syscall";
}

void Generator::writeLiterals(std::wostream &stream)
{
    for(const Identifier &id : m_tokenizer.identifiers())
    {
        if (id.context == Identifier::Context::Literal)
        {
            stream <<  hash(id.decoratedName) << L':' << std::endl;
            stream << Identifier::typeToAsm(id.type) << L'\t';
            if (id.type == Identifier::Type::String)
                stream << id.value.stringValue;
            else if (id.type == Identifier::Type::Double)
                stream << id.value.doubleValue;
            else
                stream << id.value.intValue;
            stream << std::endl;
        }
    }
}

void Generator::writeFunctions(std::wostream &stream)
{
    for(std::vector<Identifier>::const_iterator identifierIterator = m_tokenizer.identifiers().begin(); identifierIterator != m_tokenizer.identifiers().end(); ++identifierIterator)
    {
        const Identifier &identifier = *identifierIterator;

        if (identifier.type == Identifier::Type::Function)
        {
            stream << hash(identifier.decoratedName) << L':' << std::endl;
            stream << push(Register(Register::StackBase, Register::Size::Full)) << std::endl;
            stream << mov(Register(Register::StackTop, Register::Size::Full), Register(Register::StackBase, Register::Size::Full)) << std::endl;
            if (identifier.rbpOffset > 0)
            {
                stream << comment(L"Space allocate for variables") << std::endl;
                stream << sub(identifier.rbpOffset, Register(Register::StackTop, Register::Size::Full)) << std::endl;
            }
            stream << comment(L"Function body") << std::endl;
            writeFunctionBody(stream, static_cast<size_t>(identifier.tokenIndex));
            // function end
            stream << mov(Register(Register::StackBase, Register::Size::Full), Register(Register::StackTop, Register::Size::Full)) << std::endl;
            stream << pop(Register(Register::StackBase, Register::Size::Full)) << std::endl;
            stream << ret() << std::endl;
        }
    }
}

void Generator::writeFunctionBody(std::wostream &stream, const size_t startTokenIndex)
{
    bool started = false;
    const std::vector<Token> &tokens = m_tokenizer.tokens();
    std::vector<Token> currentOp;
    for(size_t i = startTokenIndex; i != m_tokenizer.tokens().size(); ++i)
    {
        if (tokens[i].token == L'v')
        {
            started = true;
            continue;
        }
        if (!started)
            continue;
        //checking for end
        if (tokens[i].token == L'}')
            break;
        // splitting
        if (tokens[i].token == L';')
        {
            if (!currentOp.empty())
            {
                writeAssembledOperation(stream, currentOp);
                currentOp.clear();
            }
        }
        else
            currentOp.push_back(tokens[i]);
    }
    if (!currentOp.empty())
        writeAssembledOperation(stream, currentOp);
}

void Generator::writeAsembledExpression(std::wostream &stream, const std::vector<Token> &operation)
{
    std::wstring sourceExpr = Token::vectorToWString(operation);
    ExpressionTokenList expToken = ExpressionToken::fromStdWString(sourceExpr);
    ExpressionTokenList converted = ExpressionToken::convertToRPN(expToken);

    std::vector<Identifier> op;

    stream << converted << std::endl;
}

void Generator::writeAssembledOperation(std::wostream &stream, const std::vector<Token> &operation)
{
    if (operation[0].token == L'i')
    {
        std::vector<Token> expression;
        std::copy(operation.begin() + 2, operation.end(), std::back_inserter(expression));
        writeAsembledExpression(stream, expression);
    }
}

void Generator::writeGlobalFunctions(std::wostream &stream)
{
    for(const Identifier &id : m_tokenizer.identifiers())
    {
        if (id.type == Identifier::Type::Function)
        {
            stream << L".globl " << hash(id.decoratedName) << std::endl;
        }
    }
}


bool Generator::isMainFunctionExists()
{
    if (m_mainFunctionExists >= 0)
        return true;
    else if (m_mainFunctionExists == -2)
        return false;

    const auto &ids = m_tokenizer.identifiers();
    for(size_t i = 0 ; i < ids.size(); ++i)
    {
        if (ids[i].type == Identifier::Type::Function && ids[i].name == L"пачатак")
        {
            m_mainFunctionExists = static_cast<int>(i);
            return true;
        }
    }
    m_mainFunctionExists = -2;
    return false;
}

std::wstring Generator::hash(const std::wstring &source)
{
    return Utils::Misc::toBase64( reinterpret_cast<const unsigned char*>(source.c_str()), source.length() * sizeof(wchar_t));
}

std::wstring Register::toString() const
{
    // rdi rsi rdx rcx r8 r9 || xmm0-7
    if (integer)
    {
        switch(argumentIndex)
        {
        case ReturnType:
        {
            switch(size)
            {
            case Size::Full:
                return L"rax";
            case Size::Half:
                return L"eax";
            case Size::Quarter:
                return L"ax";
            case Size::OneEight:
                return L"al";
            }
            break;
        }
        case StackBase:
        {
            switch(size)
            {
            case Size::Full:
                return L"rbp";
            case Size::Half:
                return L"ebp";
            case Size::Quarter:
                return L"bp";
            case Size::OneEight:
                return L"bpl";
            }
        }
        case StackTop:
        {
            switch(size)
            {
            case Size::Full:
                return L"rsp";
            case Size::Half:
                return L"esp";
            case Size::Quarter:
                return L"sp";
            case Size::OneEight:
                return L"spl";
            }
        }
        case 0:
        {
            switch(size)
            {
            case Size::Full:
                return L"rdi";
            case Size::Half:
                return L"edi";
            case Size::Quarter:
                return L"di";
            case Size::OneEight:
                return L"dil";
            }
            break;
        }
        case 1:
        {
            switch(size)
            {
            case Size::Full:
                return L"rsi";
            case Size::Half:
                return L"esi";
            case Size::Quarter:
                return L"si";
            case Size::OneEight:
                return L"sil";
            }
            break;
        }
        case 2:
        {
            switch(size)
            {
            case Size::Full:
                return L"rdx";
            case Size::Half:
                return L"edx";
            case Size::Quarter:
                return L"dx";
            case Size::OneEight:
                return L"dl";
            }
            break;
        }
        case 3:
        {
            switch(size)
            {
            case Size::Full:
                return L"rcx";
            case Size::Half:
                return L"ecx";
            case Size::Quarter:
                return L"cx";
            case Size::OneEight:
                return L"cl";
            }
            break;
        }
        case 4:
        {
            switch(size)
            {
            case Size::Full:
                return L"r8";
            case Size::Half:
                return L"r8d";
            case Size::Quarter:
                return L"r8w";
            case Size::OneEight:
                return L"r8b";
            }
            break;
        }
        case 5:
        {
            switch(size)
            {
            case Size::Full:
                return L"r9";
            case Size::Half:
                return L"r9d";
            case Size::Quarter:
                return L"r9w";
            case Size::OneEight:
                return L"r9b";
            }
            break;
        }
        }
    }
    return std::wstring();
}

Register::Register(unsigned short index, Register::Size asize) : size(asize), argumentIndex(index)
{

}


} // namespace Transducer
} // namespace PDA
