#include "expressiontoken.h"

#include <stack>

namespace PDA
{
namespace Transducer
{

ExpressionToken::ExpressionToken(const wchar_t ch) : m_charToken(ch), m_braceType(BraceType::Unassigned)
{ }

bool ExpressionToken::operator ==(const wchar_t ch)
{
    return m_charToken == ch;
}

int ExpressionToken::getPriority() const
{
    if (m_charToken == L';')
        return 1;
    else if (m_charToken == L'-' || m_charToken == L'+')
        return 2;
    else if (m_charToken == L'*' || m_charToken == L'/' || m_charToken == L'%')
        return 3;
    else if (m_charToken == L')' || m_charToken == L'(')
    {
        if (m_braceType == BraceType::Priority || m_braceType == BraceType::Unassigned)
            return 0;
        else
            return 4;
    }
    return -1;
}

bool ExpressionToken::isOperand() const
{
    return m_charToken == L'i' || m_charToken == L'l';
}

bool ExpressionToken::isOperation() const
{
    if (m_charToken == L'-' || m_charToken == L'+' || m_charToken == L'/' || m_charToken == L'*' || m_charToken == L'%')
        return true;
    else
        return false;
}

bool ExpressionToken::isOpenBrace() const
{
    return isOpenBrace(m_charToken);
}

bool ExpressionToken::isCloseBrace() const
{
    return isCloseBrace(m_charToken);
}

bool ExpressionToken::isBrace() const
{
    return isOpenBrace() || isCloseBrace();
}

bool ExpressionToken::isOpenBrace(const wchar_t wc)
{
    return wc == L'(';
}

bool ExpressionToken::isCloseBrace(const wchar_t wc)
{
    return wc == L')';
}

bool ExpressionToken::isBrace(const wchar_t wc)
{
    return isOpenBrace(wc) || isCloseBrace(wc);
}

std::list<ExpressionToken> ExpressionToken::fromStdWString(const std::wstring &source)
{
    std::list<ExpressionToken> result;
    std::stack<wchar_t> braces;
    bool isPrevIdentifier = false;
    for(size_t i = 0 ; i < source.size(); ++i)
    {
        ExpressionToken currentToken = source[i];
        if (isOpenBrace(source[i]))
        {
            if (isPrevIdentifier)
            {
                currentToken.m_braceType = BraceType::Function;
                braces.push(L'[');
            }
            else
            {
                currentToken.m_braceType = BraceType::Priority;
                braces.push(L'(');
            }
        }
        else if (isCloseBrace(source[i]))
        {
            if (braces.empty()) // Error
                return std::list<ExpressionToken>();
            if (braces.top() == L'[')
                currentToken.m_braceType = BraceType::Function;
            else if (braces.top() == L'(')
                currentToken.m_braceType = BraceType::Priority;
            else
                currentToken.m_braceType = BraceType::Unassigned;
            braces.pop();
        }
        result.push_back(currentToken);
        isPrevIdentifier = (source[i] == L'i') ? true : false;
    }
    return result;
}

std::list<ExpressionToken> ExpressionToken::convertToRPN(const std::list<ExpressionToken> &expr)
{
    std::list<ExpressionToken> result;
    std::stack<ExpressionToken> stack;
    std::stack<int> commas;
    for(const ExpressionToken &token : expr)
    {
        if(token.isOperand())
        {
            result.push_back(token);
        }
        else if (token.isOperation())
        {
            if (stack.empty() || stack.top().isOpenBrace() )
            {
                stack.push(token);
            }
            else
            {
                while(!stack.empty() && stack.top().getPriority() >= token.getPriority())
                {
                    result.push_back(stack.top());
                    stack.pop();
                }
                stack.push(token);
            }
        }
        else if (token.isComma())
        {
            ++commas.top();
            while(!stack.empty() && stack.top().isOperation())
            {
                result.push_back(stack.top());
                stack.pop();
            }
        }
        else if (token.isOpenBrace())
        {
            stack.push(token);
            if (token.getBraceType() == ExpressionToken::BraceType::Function)
                commas.push(1);
        }
        else if (token.isCloseBrace())
        {
            if (token.getBraceType() == ExpressionToken::BraceType::Priority)
            {
                while(!stack.empty() && !(stack.top().isOpenBrace() && stack.top().getBraceType() == ExpressionToken::BraceType::Priority) )
                {
                    result.push_back(stack.top());
                    stack.pop();
                }
                stack.pop();
            }
            else if (token.getBraceType() == ExpressionToken::BraceType::Function)
            {
                while(!stack.empty() && !(stack.top().isOpenBrace() && stack.top().getBraceType() == ExpressionToken::BraceType::Function) )
                {
                    result.push_back(stack.top());
                    stack.pop();
                }
                stack.pop();
                result.push_back(L'@');
                const std::wstring argumentsCountStr = std::to_wstring(commas.top());
                std::copy(argumentsCountStr.begin(), argumentsCountStr.end(), std::back_inserter(result));
                commas.pop();
            }

        }
    }
    while(!stack.empty())
    {
        result.push_back(stack.top());
        stack.pop();
    }
    return result;
}

bool ExpressionToken::isComma() const
{
    return m_charToken == L',';
}

wchar_t ExpressionToken::getCharToken() const
{
    return m_charToken;
}

ExpressionToken::BraceType ExpressionToken::getBraceType() const
{
    return m_braceType;
}

std::wostream &operator <<(std::wostream &stream, const ExpressionToken::BraceType braceType)
{
    switch(braceType)
    {
    case ExpressionToken::BraceType::Unassigned:
        stream << L"ExpressionToken::BraceType::Unassigned";
        break;
    case ExpressionToken::BraceType::Function:
        stream << L"ExpressionToken::BraceType::Function";
        break;
    case ExpressionToken::BraceType::Priority:
        stream << L"ExpressionToken::BraceType::Priority";
        break;
    }
    return stream;
}

std::wostream &operator <<(std::wostream &stream, const ExpressionTokenList &expList)
{
    std::wstring result;
    for(const auto &i : expList)
        result += i.getCharToken();
    stream << result;
    return stream;
}

} // namespace Transducer
} // namespace PDA
