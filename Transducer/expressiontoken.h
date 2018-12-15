#ifndef EXPRESSIONTOKEN_H
#define EXPRESSIONTOKEN_H

#include <string>
#include <list>
#include <ostream>

namespace PDA
{
namespace Transducer
{

class ExpressionToken
{
public:
    enum class BraceType
    {
        Unassigned,
        Function,
        Priority
    };
    ExpressionToken(const wchar_t ch);
    bool operator == (const wchar_t ch);
    int getPriority() const;
    wchar_t getCharToken() const;
    BraceType getBraceType() const;

    static bool isOpenBrace(const wchar_t wc);
    static bool isCloseBrace(const wchar_t wc);
    static bool isBrace(const wchar_t wc);
    static std::list<ExpressionToken> fromStdWString(const std::wstring &source);
    static std::list<ExpressionToken> convertToRPN(const std::list<ExpressionToken> &expr);

    bool isComma() const;
    bool isOperand() const;
    bool isOperation() const;
    bool isOpenBrace() const;
    bool isCloseBrace() const;
    bool isBrace() const;
private:
    wchar_t m_charToken;
    BraceType m_braceType;
};
typedef std::list<ExpressionToken> ExpressionTokenList;

std::wostream &operator << (std::wostream &stream, const ExpressionToken::BraceType braceType);
std::wostream &operator << (std::wostream &stream, const ExpressionTokenList &expList);

} // namespace Transducer
} // namespace PDA

#endif // EXPRESSIONTOKEN_H
