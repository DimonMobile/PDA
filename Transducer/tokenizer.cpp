#include "tokenizer.h"

#include <cwctype>
#include <iostream>
#include <iomanip>
#include <cwctype>

#include "../Utils/settings.h"
#include "../Utils/defaults.h"
#include "../Exception/source_exception.h"

namespace PDA
{
namespace Transducer
{

Tokenizer::Tokenizer(const std::wstring &source) : m_willFunction(false), m_isPrevTokenTypeToken(false)
{
    int preprocReadingStatus(0);
    std::wstring readedLineIndex, tokenFileName;
    std::string currentFileS(PDA::Utils::Settings::Instance().sourceFilePath());
    m_currentFile = std::wstring(currentFileS.begin(), currentFileS.end());
    m_currentPos  = 0;
    m_currentLine = 1;
    bool isStartLine{true}, isPreprocLine{false};
    bool quoteStarted{false};
    m_fst = Utils::Defaults::fst();


    for(const wchar_t currentChar : source)
    {
        if (isStartLine)
        {
            isStartLine = false;
            if (currentChar == L'@')
            {
                isPreprocLine = true;
            }
        }
        else if (isPreprocLine)
        {
            if (preprocReadingStatus == 0)
            {
                if (currentChar == L' ')
                {
                    ++preprocReadingStatus;
                }
            }
            else if (preprocReadingStatus == 1)
            {
                if (currentChar != L' ')
                {
                    readedLineIndex += currentChar;
                }
                else
                {
                    ++preprocReadingStatus;
                }
            }
            else if (preprocReadingStatus == 2)
            {
                if (currentChar != L'\n')
                {
                    tokenFileName += currentChar;
                }
                else
                {
                    preprocReadingStatus = 0;
                    m_currentFile = tokenFileName;
                    m_currentPos = 0;
                    m_currentLine = std::stoi(readedLineIndex);

                    tokenFileName.clear();
                    readedLineIndex.clear();
                    isPreprocLine = false;
                }
            }
        }

        if (isPreprocLine)
            continue;

        ++m_currentPos;
        if (currentChar == '\n')
        {
            ++m_currentLine;
            m_currentPos = 0;
            isStartLine = true;
        }


        if (quoteStarted && currentChar != '\"')
        {
            if (currentChar == '\n') // newline on quote
            {
                throw Exception::SourceException(m_currentLine, m_currentPos, m_currentFile + L" didn't closed \"(did you forge to close your string literal?)");
            }
            m_token += currentChar;
        }
        else if ( iswspace(static_cast<wint_t>(currentChar)) )
        {
            commitToken();
        }
        else if (currentChar == '\"')
        {
            if (!quoteStarted)
            {
                commitToken();
                m_token += currentChar;
                quoteStarted = true;
            }
            else
            {
                m_token += currentChar;
                commitToken();
                quoteStarted = false;
            }
        }
        else if (currentChar == ':' || currentChar == ';' || currentChar == ')' || currentChar == '(')
        {
            if (!m_token.empty())
                commitToken();
            m_token += currentChar;
            commitToken();
        }
        else
        {
            if (!m_token.empty())
            {
                if ( iswalnum(static_cast<wint_t>( m_token[m_token.size() - 1] )) || m_token[m_token.size() - 1] == L'_' )
                {
                    if ( !iswalnum(static_cast<wint_t>(currentChar)) && currentChar != L'_' )
                    {
                        commitToken();
                    }
                }
                else
                {
                    if ( iswalnum(static_cast<wint_t>(currentChar)) || currentChar == L'_' )
                    {
                        commitToken();
                    }
                }
            }
            m_token += currentChar;
        }
    }
}

void Tokenizer::commitToken()
{
    if (m_token.empty())
        return;
    if (!m_fst.execute(m_token))
        throw Exception::SourceException(m_currentLine, m_currentPos, L'\"' + m_currentFile + L"\" unknown lexeme: " + m_token);
    if (Fst::userData == L's')
        Fst::userData = L't';


    Token currentToken;
    if (!m_files.empty())
    {
        if (m_files[m_files.size() - 1] != m_currentFile)
            m_files.push_back(m_currentFile);
    }
    else
        m_files.push_back(m_currentFile);
    currentToken.fileIndex = static_cast<int>(m_files.size() - 1);
    currentToken.line = m_currentLine;
    currentToken.position = m_currentPos;
    currentToken.token = Fst::userData;
    m_tokens.push_back(currentToken);


    if (currentToken.token == L't')
    {
        if (!m_identifiers.empty())
        {
            if (!m_isPrevTokenTypeToken)
            {
                m_identifiers[m_identifiers.size()-1].type = Identifier::typeFromString(m_token);
            }
            else
            {
                if (m_lastFunctionTokenIndex < static_cast<int>(m_identifiers.size())) // range check
                {
                    m_identifiers[m_lastFunctionTokenIndex].returnType = Identifier::typeFromString(m_token);
                }
            }
        }
        m_isPrevTokenTypeToken = true;
    }
    else if (currentToken.token == L'i')
    {
        m_isPrevTokenTypeToken = false;
        Identifier id;
        id.tokenIndex = static_cast<int>(m_tokens.size() - 1);
        id.name = m_token;
        if (m_willFunction)
        {
            m_willFunction = false;
            id.type = Identifier::Type::Function;
            m_lastFunctionTokenIndex = static_cast<int>(m_identifiers.size());
        }
        m_identifiers.push_back(id);
    }
    else if (currentToken.token == L'f')
    {
        m_willFunction = true;
    }
    m_token.clear();
}

void Tokenizer::printTokens()
{
    std::wcout << "================LEX TABLE=====================" << std::endl;
    int lastLine(1);
    for(Token &token : m_tokens)
    {
        if (token.line != lastLine)
            std::wcout << std::endl;
        std::wcout << token.token;
        lastLine = token.line;
    }
    std::wcout << std::endl;
    std::wcout << "=============================================" << std::endl;
}

void Tokenizer::printIdentifiers()
{
    std::wcout << "================Identifiers==================" << std::endl;
    for(Identifier &identifier : m_identifiers)
    {
        std::wcout << std::setw(10) << Identifier::typeToString(identifier.type) << std::setw(10) << identifier.name << " ->" << std::setw(10) << Identifier::typeToString(identifier.returnType) << std::setw(3) << m_tokens[identifier.tokenIndex].line << std::setw(3) << m_tokens[identifier.tokenIndex].position - identifier.name.size() << std::setw(80) <<  m_files[m_tokens[identifier.tokenIndex].fileIndex] << std::endl;
    }
    std::wcout << "=============================================" << std::endl;
}

std::vector<Token> &Tokenizer::tokens()
{
    return m_tokens;
}

std::vector<std::wstring> &Tokenizer::files()
{
    return m_files;
}

std::wstring Identifier::typeToString(const Identifier::Type tp)
{
    switch(tp)
    {
    case Type::Bool:
        return L"лагiчнае";
    case Type::Double:
        return L"дробнае";
    case Type::Function:
        return L"функцыя";
    case Type::Integer:
        return L"цэлае";
    case Type::String:
        return L"радок";
    }
    return std::wstring();
}

Identifier::Type Identifier::typeFromString(const std::wstring &tp)
{
    Type type = Type::Integer;
    if (tp == L"цэлае")
        type = Type::Integer;
    else if (tp == L"дробнае")
        type = Type::Double;
    else if (tp == L"будзе")
        type = Type::Function;
    else if (tp == L"радок")
        type = Type::String;
    else if (tp == L"лагiчнае")
        type = Type::Bool;
    else if (tp == L"стварыць")
        type = Type::Function;
    return type;
}

int Identifier::size()
{
    switch(type)
    {
    case Type::Integer:
        return 4;
    case Type::Bool:
        return 1;
    case Type::String:
        return 0;
    case Type::Double:
        return 8;
    case Type::Function:
        return 0;
    }
    return 0;
}

} // namespace Transducer
} // namespace PDA
