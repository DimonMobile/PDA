#include "generator.h"

#include <string>
#include <fstream>
#include <cassert>

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
    const std::wstring integerOutputStringLabel = L"IntegerOutput";
    const std::wstring integerOutputString = L".asciz \"%d\\n\"";
}

Generator::Generator(const Tokenizer &tokenizer, const StoreFst &storeFst) :
    m_mainFunctionExists(-1), m_tokenizer(tokenizer),
    m_storeFst(storeFst)
{

    std::wofstream ostream("result.s");
    ostream << Constants::firstLineComment << std::endl;
    ostream << Constants::sectionDataString << std::endl;
    ostream << Constants::integerOutputStringLabel << L':' << std::endl;
    ostream << Constants::integerOutputString << std::endl;
    //writeLiterals(ostream);
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

std::wstring Generator::cltd()
{
    return L"cltd";
}

std::wstring Generator::div(const Register &dest, const wchar_t suff)
{
    std::wstring result = L"idiv";
    result += suff == L'\0' ? registerSuffix(dest) : suff;
    result += L"\t";
    result += dest.toString();
    return result;
}

std::wstring Generator::mul(const Register &source, const Register &dest, const wchar_t suff)
{
    std::wstring result = L"imul";
    result += suff == L'\0' ? registerSuffix(source) : suff;
    result += L"\t";
    result += source.toString();
    result += L",\t";
    result += dest.toString();
    return result;
}

std::wstring Generator::add(const Register &source, const Register &dest, const wchar_t suff)
{
    std::wstring result = L"add";
    result += suff == L'\0' ? registerSuffix(source) : suff;
    result += L"\t";
    result += source.toString();
    result += L",\t";
    result += dest.toString();
    return result;
}

std::wstring Generator::add(const int source, const Register &dest, const wchar_t suff)
{
    std::wstring result = L"add";
    result += suff == L'\0' ? registerSuffix(dest) : suff;
    result += L'\t';
    result += L'$' + std::to_wstring(source);
    result += L",\t" + dest.toString();
    return result;
}


std::wstring Generator::sub(const Register &source, const Register &dest, const wchar_t suff)
{
    std::wstring result = L"sub";
    result += suff == L'\0' ? registerSuffix(source) : suff;
    result += L"\t";
    result += source.toString();
    result += L",\t";
    result += dest.toString();
    return result;
}

std::wstring Generator::sub(const int source, const Register &dest, const wchar_t suff)
{
    std::wstring result = L"sub";
    result += suff == L'\0' ? registerSuffix(dest) : suff;
    result += L'\t';
    result += L'$' + std::to_wstring(source);
    result += L",\t" + dest.toString();

    return result;
}

std::wstring Generator::sub(const Register &source, const int dest, const wchar_t suff)
{
    std::wstring result = L"sub";
    result += suff == L'\0' ? registerSuffix(source) : suff;
    result += L"\t";
    result += source.toString();
    result += L",\t";
    result += L"$" + std::to_wstring(dest);
    return result;
}

std::wstring Generator::sub(const Register &source, const std::wstring &dest, const wchar_t suff)
{
    std::wstring result = L"sub";
    result += suff == L'\0' ? registerSuffix(source) : suff;
    result += L"\t";
    result += source.toString();
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

std::wstring Generator::push(const Register &source, const wchar_t suff)
{
    std::wstring result = L"push";
    result += suff == L'\0' ? registerSuffix(source) : suff;
    result += L'\t';
    result += source.toString();
    return result;
}

std::wstring Generator::pop(const Register &dest, const wchar_t suff)
{
    std::wstring result = L"pop";
    result += suff == L'\0' ? registerSuffix(dest) : suff;
    result += L'\t';
    result += dest.toString();
    return result;
}

std::wstring Generator::mov(const Register &source, const Register &dest, const wchar_t suff)
{
    std::wstring result = L"mov";
    result += suff == L'\0' ? registerSuffix(source) : suff;
    result += L"\t";
    result += source.toString();
    result += L",\t";
    result += dest.toString();
    return result;
}

std::wstring Generator::mov(const std::wstring &source, const Register &dest, const wchar_t suff)
{
    std::wstring result = L"mov";
    result += suff == L'\0' ? registerSuffix(dest) : suff;
    result += L"\t" + source;
    result += L",\t" + dest.toString();
    return result;
}

std::wstring Generator::mov(const int source, const Register &dest, const wchar_t suff)
{
    std::wstring result = L"mov";
    result += suff == L'\0' ? registerSuffix(dest) : suff;
    result += L"\t$" + std::to_wstring(source);
    result += L",\t" + dest.toString();
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
            stream << comment(L"Function begin") << std::endl;
            stream << hash(identifier.decoratedName) << L':' << std::endl;
            stream << push(Register(Register::StackBase, Register::Size::Full)) << std::endl;
            stream << mov(Register(Register::StackTop, Register::Size::Full), Register(Register::StackBase, Register::Size::Full)) << std::endl;
            if (identifier.rbpOffset > 0)
            {
                stream << comment(L"Space allocate for variables") << std::endl;
                stream << sub( identifier.roundedRbpOffset() , Register(Register::StackTop, Register::Size::Full)) << std::endl;
                stream << comment(L"Arguments copy from registers to variables") << std::endl;
                // copying arguments
                for(size_t i = 0 ; i < identifier.functionArgs.size(); ++i)
                {
                    if (identifier.functionArgs[i].type == Identifier::Type::Integer)
                    {
                        stream << mov(Register(i, Register::Size::Half), Register(Register::StackBase, Register::Size::Full, identifier.functionArgs[i].rbpOffset), L'l') << std::endl;
                    }
                }
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

Identifier::Type Generator::writeAsembledExpression(std::wostream &stream, const std::vector<Token> &operation)
{
    Identifier::Type result = Identifier::Type::Undefined;
    std::wstring sourceExpr = Token::vectorToWString(operation);
    ExpressionTokenList expToken = ExpressionToken::fromStdWString(sourceExpr);
    ExpressionTokenList converted = ExpressionToken::convertToRPN(expToken);

    std::stack<Identifier> idStack;
    int offset = -1;

    for(std::list<ExpressionToken>::const_iterator tokenIterator = converted.cbegin(); tokenIterator != converted.cend(); ++tokenIterator)
    {
        const ExpressionToken &token = *tokenIterator;
        if (token.isOperand())
        {
            offset = Token::getNextIdentifierIdx(operation, offset + 1);

            const Identifier &currentIdentifier = m_tokenizer.identifiers()[static_cast<size_t>(operation[static_cast<size_t>(offset)].identifierIdx)];
            idStack.push(currentIdentifier);
            switch(currentIdentifier.type)
            {
            case Identifier::Type::Integer:
            {
                stream << sub(4, Register(Register::StackTop, Register::Size::Full)) << std::endl;
                if (currentIdentifier.context == Identifier::Context::Literal)
                {
                    stream << mov(currentIdentifier.value.intValue, Register(Register::StackTop, Register::Size::Full, 0), L'l') << std::endl;
                }
                else if (currentIdentifier.context == Identifier::Context::Link)
                {
                    stream << mov(Register(Register::StackBase, Register::Size::Full, m_tokenizer.identifiers()[currentIdentifier.linkTo].rbpOffset), Register(Register::ReturnType, Register::Size::Half), L'l') << std::endl;
                    stream << mov(Register(Register::ReturnType, Register::Size::Half), Register(Register::StackTop, Register::Size::Full, 0), L'l') << std::endl;
                }
                result = Identifier::Type::Integer;
                break;
            }
            }
        }
        else if (token.getCharToken() == L'@')
        {
            int argCount = (++tokenIterator)->getCharToken() - L'0';
            std::vector<Identifier> arguments;
            // TODO: function passing arguments

        }
        else if (token.isOperation())
        {
            switch(idStack.top().type)
            {
            case Identifier::Type::Integer:
            {
                if (token.getCharToken() == L'+')
                {
                    stream << comment(L"sum") << std::endl;
                    stream << mov(Register(Register::StackTop, Register::Size::Full, 0), Register(Register::Source, Register::Size::Half), L'l') << std::endl;
                    // stack poping
                    stream << add(4, Register(Register::StackTop, Register::Size::Full)) << std::endl;
                    idStack.pop();
                    if (idStack.top().type != Identifier::Type::Integer)
                    {
                        // TODO: throw exception: invalid cast
                    }
                    stream << mov(Register(Register::StackTop, Register::Size::Full, 0), Register(Register::Dest, Register::Size::Half), L'l') << std::endl;
                    stream << add(Register(Register::Source, Register::Size::Half), Register(Register::Dest, Register::Size::Half)) << std::endl;
                    stream << mov(Register(Register::Dest, Register::Size::Half), Register(Register::StackTop, Register::Size::Full, 0), L'l') << std::endl;
                    result = Identifier::Type::Integer;
                }
                else if (token.getCharToken() == L'-')
                {
                    stream << comment(L"sub") << std::endl;
                    stream << mov(Register(Register::StackTop, Register::Size::Full, 0), Register(Register::Source, Register::Size::Half), L'l') << std::endl;
                    // stack poping
                    stream << add(4, Register(Register::StackTop, Register::Size::Full)) << std::endl;
                    idStack.pop();
                    if (idStack.top().type != Identifier::Type::Integer)
                    {
                        // TODO: throw exception: invalid cast
                    }
                    stream << mov(Register(Register::StackTop, Register::Size::Full, 0), Register(Register::Dest, Register::Size::Half), L'l') << std::endl;
                    stream << sub(Register(Register::Source, Register::Size::Half), Register(Register::Dest, Register::Size::Half)) << std::endl;
                    stream << mov(Register(Register::Dest, Register::Size::Half), Register(Register::StackTop, Register::Size::Full, 0), L'l') << std::endl;
                    result = Identifier::Type::Integer;
                }
                else if (token.getCharToken() == L'*')
                {
                    stream << comment(L"imul") << std::endl;
                    stream << mov(Register(Register::StackTop, Register::Size::Full, 0), Register(Register::Source, Register::Size::Half), L'l') << std::endl;
                    // stack poping
                    stream << add(4, Register(Register::StackTop, Register::Size::Full)) << std::endl;
                    idStack.pop();
                    if (idStack.top().type != Identifier::Type::Integer)
                    {
                        // TODO: throw exception: invalid cast
                    }
                    stream << mov(Register(Register::StackTop, Register::Size::Full, 0), Register(Register::Dest, Register::Size::Half), L'l') << std::endl;
                    stream << mul(Register(Register::Source, Register::Size::Half), Register(Register::Dest, Register::Size::Half)) << std::endl;
                    stream << mov(Register(Register::Dest, Register::Size::Half), Register(Register::StackTop, Register::Size::Full, 0), L'l') << std::endl;
                    result = Identifier::Type::Integer;
                }
                else if (token.getCharToken() == L'/')
                {
                    stream << comment(L"idiv") << std::endl;
                    stream << mov(Register(Register::StackTop, Register::Size::Full, 0), Register(Register::Dest, Register::Size::Half), L'l') << std::endl;
                    // stack poping
                    stream << add(4, Register(Register::StackTop, Register::Size::Full)) << std::endl;
                    idStack.pop();
                    if (idStack.top().type != Identifier::Type::Integer)
                    {
                        // TODO: throw exception: invalid cast
                    }
                    stream << mov(Register(Register::StackTop, Register::Size::Full, 0), Register(Register::ReturnType, Register::Size::Half), L'l') << std::endl;
                    stream << cltd() << std::endl;
                    stream << div(Register(Register::Dest, Register::Size::Half)) << std::endl;
                    stream << mov(Register(Register::ReturnType, Register::Size::Half), Register(Register::StackTop, Register::Size::Full, 0), L'l') << std::endl;
                    result = Identifier::Type::Integer;
                }
                else if (token.getCharToken() == L'%')
                {
                    stream << comment(L"idiv(remainder)") << std::endl;
                    stream << mov(Register(Register::StackTop, Register::Size::Full, 0), Register(Register::Dest, Register::Size::Half), L'l') << std::endl;
                    // stack poping
                    stream << add(4, Register(Register::StackTop, Register::Size::Full)) << std::endl;
                    idStack.pop();
                    if (idStack.top().type != Identifier::Type::Integer)
                    {
                        // TODO: throw exception: invalid cast
                    }
                    stream << mov(Register(Register::StackTop, Register::Size::Full, 0), Register(Register::ReturnType, Register::Size::Half), L'l') << std::endl;
                    stream << cltd() << std::endl;
                    stream << div(Register(Register::Dest, Register::Size::Half)) << std::endl;
                    stream << mov(Register(2, Register::Size::Half), Register(Register::StackTop, Register::Size::Full, 0), L'l') << std::endl;
                    result = Identifier::Type::Integer;
                }
                break;
            }
            }
        }
    }
    return result;
}

void Generator::writeAssembledOperation(std::wostream &stream, const std::vector<Token> &operation)
{
    if (operation[0].token == L'i')
    {
        std::vector<Token> expression;
        std::copy(operation.begin() + 2, operation.end(), std::back_inserter(expression));
        Identifier::Type expressionResultType = writeAsembledExpression(stream, expression);
        assert(expressionResultType != Identifier::Type::Undefined); // TODO: throw exception
        if (expressionResultType == Identifier::Type::Integer)
        {
            stream << comment(L"retrieve value and restore stack") << std::endl;
            stream << mov(Register(Register::StackTop, Register::Size::Full, 0), Register(Register::Dest, Register::Size::Half), L'l') << std::endl;
            stream << mov(Register(Register::Dest, Register::Size::Half), Register(Register::StackBase, Register::Size::Full, m_tokenizer.identifiers()[m_tokenizer.identifiers()[operation[0].identifierIdx].linkTo].rbpOffset ), L'l') << std::endl;
            stream << add(4, Register(Register::StackTop, Register::Size::Full)) << std::endl;
        }
    }
    else if (operation[0].token == L'p')
    {
        std::vector<Token> expression;
        std::copy(operation.begin() + 1, operation.end(), std::back_inserter(expression));
        Identifier::Type expressionResultType = writeAsembledExpression(stream, expression);
        stream << comment(L"Printing") << std::endl;

        switch(expressionResultType)
        {
        case Identifier::Type::Integer:
            stream << mov( L'$' + Constants::integerOutputStringLabel, Register(0, Register::Size::Full) ) << std::endl;
            stream << mov( Register(Register::StackTop, Register::Size::Full, 0), Register(1, Register::Size::Half), L'l') << std::endl;
            stream << add(4, Register(Register::StackTop, Register::Size::Full)) << std::endl;
            break;
        default:
            throw Exception::Exception(); // TODO: revise exception
        }
        stream << call(L"printf") << std::endl;
    }
    else if (operation[0].token == L'r')
    {
        std::vector<Token> expression;
        std::copy(operation.begin() + 1, operation.end(), std::back_inserter(expression));
        Identifier::Type expressionResultType = writeAsembledExpression(stream, expression);
        stream << comment(L"Returning") << std::endl;
        switch (expressionResultType) {
        case Identifier::Type::Integer:
            stream << mov(Register(Register::StackTop, Register::Size::Full, 0), Register(Register::ReturnType, Register::Size::Half), L'l') << std::endl;
            stream << add(4, Register(Register::StackTop, Register::Size::Full)) << std::endl;
            break;
        default:
            throw Exception::Exception(); // TODO: revice exception
        }
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
    std::wstring result;
    if (integer)
    {
        switch(argumentIndex)
        {
        case ReturnType:
        {
            switch(size)
            {
            case Size::Full:
                result = L"rax";
                break; case Size::Half:
                result = L"eax";
                break; case Size::Quarter:
                result = L"ax";
                break; case Size::OneEight:
                result = L"al";
            }
            break;
        }
        case StackBase:
        {
            switch(size)
            {
            case Size::Full:
                result = L"rbp";
                break; case Size::Half:
                result = L"ebp";
                break; case Size::Quarter:
                result = L"bp";
                break; case Size::OneEight:
                result = L"bpl";
            }
            break;
        }
        case StackTop:
        {
            switch(size)
            {
            case Size::Full:
                result = L"rsp";
                break; case Size::Half:
                result = L"esp";
                break; case Size::Quarter:
                result = L"sp";
                break; case Size::OneEight:
                result = L"spl";
            }
            break;
        }
        case 0:
        {
            switch(size)
            {
            case Size::Full:
                result = L"rdi";
                break; case Size::Half:
                result = L"edi";
                break; case Size::Quarter:
                result = L"di";
                break; case Size::OneEight:
                result = L"dil";
            }
            break;
        }
        case 1:
        {
            switch(size)
            {
            case Size::Full:
                result = L"rsi";
                break; case Size::Half:
                result = L"esi";
                break; case Size::Quarter:
                result = L"si";
                break; case Size::OneEight:
                result = L"sil";
            }
            break;
        }
        case 2:
        {
            switch(size)
            {
            case Size::Full:
                result = L"rdx";
                break; case Size::Half:
                result = L"edx";
                break; case Size::Quarter:
                result = L"dx";
                break; case Size::OneEight:
                result = L"dl";
            }
            break;
        }
        case 3:
        {
            switch(size)
            {
            case Size::Full:
                result = L"rcx";
                break; case Size::Half:
                result = L"ecx";
                break; case Size::Quarter:
                result = L"cx";
                break; case Size::OneEight:
                result = L"cl";
            }
            break;
        }
        case 4:
        {
            switch(size)
            {
            case Size::Full:
                result = L"r8";
                break; case Size::Half:
                result = L"r8d";
                break; case Size::Quarter:
                result = L"r8w";
                break; case Size::OneEight:
                result = L"r8b";
            }
            break;
        }
        case 5:
        {
            switch(size)
            {
            case Size::Full:
                result = L"r9";
                break; case Size::Half:
                result = L"r9d";
                break; case Size::Quarter:
                result = L"r9w";
                break; case Size::OneEight:
                result = L"r9b";
            }
            break;
        }
        }
    }

    if (!result.empty())
    {
        result = L'%' + result;
        if (isPtr)
        {
            result = L'(' + result + L')';
            if (ptrOffset != 0)
                result = std::to_wstring(ptrOffset) + result;
        }
    }

    return result;
}

Register::Register(unsigned short index, Register::Size asize) : size(asize), argumentIndex(index)
{
}

Register::Register(unsigned short index, Register::Size assize, int offset) : size(assize), argumentIndex(index), ptrOffset(offset)
{
    isPtr = true;
}


} // namespace Transducer
} // namespace PDA
