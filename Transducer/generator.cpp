#include "generator.h"

#include <string>
#include <fstream>

#define UNUSED(x) (void)x

namespace PDA
{
namespace Transducer
{

namespace Constants
{
    const std::wstring firstLineComment = L"# PDA v1.0 Complier; LGPL3 https://www.gnu.org/licenses/lgpl-3.0.en.html ";
    const std::wstring sectionDataString = L".section .data";
    const std::wstring sectionUninitializedDataString = L".section .bss";
    const std::wstring sectionTextString = L".section .text";
}

Generator::Generator(const Tokenizer &tokenizer, const StoreFst &storeFst)
{
    UNUSED(tokenizer);
    UNUSED(storeFst);

    std::wofstream ostream("result.s");
    ostream << Constants::firstLineComment << std::endl;
    ostream << Constants::sectionDataString << std::endl;
    ostream << Constants::sectionUninitializedDataString << std::endl;
    ostream << Constants::sectionTextString << std::endl;
    ostream << mov(L"$60", Register(Register::ReturnType, Register::Size::Full)) << std::endl;
    ostream << mov(L"$0", Register(0, Register::Size::Full)) << std::endl;
    ostream << syscall() << std::endl;
}

std::wstring Generator::mov(const Register &source, const Register &destination)
{
    std::wstring result = L"mov";
    if (source.integer)
    {
        switch(source.size)
        {
        case Register::Size::Full:
            result += L'q';
            break;
        case Register::Size::Half:
            result += L'l';
            break;
        case Register::Size::Quarter:
            result += L'w';
            break;
        case Register::Size::OneEight:
            result += L'b';
            break;
        }
    }
    result += L"\t%";
    result += source.toString();
    result += L",\t%";
    result += destination.toString();
    return result;
}

std::wstring Generator::mov(const std::wstring &source, const Register &destination)
{
    std::wstring result = L"mov";
    if (destination.integer)
    {
        switch(destination.size)
        {
        case Register::Size::Full:
            result += L'q';
            break;
        case Register::Size::Half:
            result += L'l';
            break;
        case Register::Size::Quarter:
            result += L'w';
            break;
        case Register::Size::OneEight:
            result += L'b';
            break;
        }
    }
    result += L"\t" + source;
    result += L",\t%" + destination.toString();
    return result;
}

std::wstring Generator::syscall()
{
    return L"syscall";
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
