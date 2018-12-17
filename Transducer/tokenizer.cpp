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

Tokenizer::Tokenizer(const std::wstring &source) : m_willFunction(false),   m_isPrevTokenTypeToken(false), m_isFunctionArgs(false), m_lastFunctionIdentifierIndex(-1)
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

    for(size_t i = 0 ; i < m_identifiers.size(); ++i)
    {
        if(m_identifiers[i].context == Identifier::Context::Default)
        {
            int foundIndex = findDeclaration(m_identifiers[i].decoratedName);
            if (foundIndex != -1)
            {
                m_identifiers[i].linkTo = foundIndex;
                m_identifiers[i].context = Identifier::Context::Link;
                m_identifiers[i].type = m_identifiers[static_cast<size_t>(foundIndex)].type;
            }
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
    else if (Fst::userData == L'm')
        Fst::userData = L'i';

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

                if (m_isFunctionArgs && m_lastFunctionIdentifierIndex >= 0 && m_lastFunctionIdentifierIndex < static_cast<int>(m_identifiers.size()))
                {
                    Identifier &identifier = m_identifiers[static_cast<size_t>(m_lastFunctionIdentifierIndex)];
                    identifier.decoratedName = identifier.decoratedName + Identifier::typeToWChar(m_identifiers[m_identifiers.size()-1].type) + L'@';
                    identifier.functionArgTypes.push_back(m_identifiers[m_identifiers.size()-1].type);
                }
            }
            else
            {
                if (m_lastFunctionIdentifierIndex >= 0 && m_lastFunctionIdentifierIndex < static_cast<int>(m_identifiers.size())) // range check
                {
                    m_identifiers[static_cast<size_t>(m_lastFunctionIdentifierIndex)].returnType = Identifier::typeFromString(m_token);
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
        m_tokens[m_tokens.size()-1].identifierIdx = static_cast<int>(m_identifiers.size());
        id.name = m_token;
        if (m_willFunction)
        {
            m_willFunction = false;
            m_isFunctionArgs = true;
            m_willFunctionName = id.name;
            id.type = Identifier::Type::Function; // sum(1, sum(2, sum(3, sum(4,5)))*5)
            id.context = Identifier::Context::Declaration;
            m_lastFunctionIdentifierIndex = static_cast<int>(m_identifiers.size());
            id.decoratedName = id.name + L'@';
        }
        else if (m_isFunctionArgs)
        {
            id.context = Identifier::Context::Argument;
        }
        else
        {
            id.decoratedName = id.name + L'@' + m_willFunctionName;
        }
        m_identifiers.push_back(id);
    }
    else if (currentToken.token == L')')
    {
        if (m_isFunctionArgs && m_lastFunctionIdentifierIndex >= 0 && m_lastFunctionIdentifierIndex < static_cast<int>(identifiers().size()))
        {
            m_isPrevTokenTypeToken = true;
            m_isFunctionArgs = false;
            m_willFunctionName = m_identifiers[static_cast<size_t>(m_lastFunctionIdentifierIndex)].decoratedName;
            for(int i = static_cast<int>(m_identifiers.size()) - 1; i >= 0; --i)
            {
                Identifier &identifier = m_identifiers[static_cast<size_t>(i)];
                if (identifier.context == Identifier::Context::Argument)
                    identifier.decoratedName = identifier.name + L'@' + m_willFunctionName;
                else
                    break;
            }
        }
    }
    else if (currentToken.token == L'v')
    {
        int summaryOffset = 0;
        for(int i = static_cast<int>(m_identifiers.size()) - 1; i >= 0; --i )
        {
            Identifier &identifier = m_identifiers[static_cast<size_t>(i)];
            if (identifier.context != Identifier::Context::Argument && identifier.type != Identifier::Type::Function)
            {
                identifier.context = Identifier::Context::Declaration;
                summaryOffset += identifier.size();
                identifier.rbpOffset = -summaryOffset;
            }
            else
            {
                break;
            }
        }
        if (m_lastFunctionIdentifierIndex >= 0 && m_lastFunctionIdentifierIndex < static_cast<int>(identifiers().size()))
        {
            m_identifiers[static_cast<size_t>(m_lastFunctionIdentifierIndex)].rbpOffset = summaryOffset;
        }
    }
    else if (currentToken.token == L'f')
    {
        m_willFunction = true;
    }
    else if (currentToken.token == L'l')
    {
        Identifier id;
        id.context = Identifier::Context::Literal;
        id.tokenIndex = static_cast<int>(m_tokens.size()) - 1;
        m_tokens[m_tokens.size()-1].identifierIdx = static_cast<int>(m_identifiers.size());
        id.type = Identifier::typeFromWChar(Fst::userData_1);
        id.decoratedName = Identifier::typeToWChar(id.type);
        id.decoratedName += L'@' + std::to_wstring(m_tokens[id.tokenIndex].line) + L'@' + std::to_wstring(m_tokens[id.tokenIndex].position);
        if (id.type == Identifier::Type::String)
        {
            id.value.stringValue = m_token;
            id.value.intValue = static_cast<long long>(m_token.size());
        }
        else if (id.type == Identifier::Type::Integer)
        {
            id.value.intValue = std::stoll(m_token);
        }
        else if (id.type == Identifier::Type::Double)
        {
            id.value.doubleValue = std::stod(m_token);
        }
        m_identifiers.push_back(id);
    }
    m_token.clear();
}

void Tokenizer::printTokens() const
{
    std::wcout << "================LEX TABLE=====================" << std::endl;
    int lastLine(1);
    for(const Token &token : m_tokens)
    {
        if (token.line != lastLine)
            std::wcout << std::endl;
        std::wcout << token.token;
        lastLine = token.line;
    }
    std::wcout << std::endl;
    std::wcout << "=============================================" << std::endl;
}

void Tokenizer::printIdentifiers() const
{
    std::wcout << std::setfill(L'=') << std::setw(190) << L'\n' << std::setfill(L' ');
    std::wcout << std::left << std::setw(5) << L"ID" << std::setw(10) << L"TYPE" << std::setw(15) << L"CONTEXT" << std::setw(10) << L"RBPOFFSET" << std::setw(10) << L"NAME" << std::setw(20) << L"DECORATED"
               << std::setw(30) << L"VALUE" << std::setw(10) << L"RETTYPE" << std::setw(5) << L"ROW" << std::setw(5) << L"COL"
               << std::setw(80) << L"FILENAME" << std::endl;
    int id{0};
    for(const Identifier &identifier : m_identifiers)
    {
        std::wcout << std::setw(5) << id++ << std::setw(10) << Identifier::typeToString(identifier.type) << std::setw(15) << Identifier::contextToString(identifier.context)
                   << std::setw(10) << identifier.rbpOffset << std::setw(10) << identifier.name
                   << std::setw(20) << identifier.decoratedName << std::setw(30);
        if (identifier.context == Identifier::Context::Literal)
        {
            if (identifier.type == Identifier::Type::String)
                std::wcout << identifier.value.stringValue;
            else if (identifier.type == Identifier::Type::Integer || identifier.type == Identifier::Type::Bool)
                std::wcout << identifier.value.intValue;
            else if (identifier.type == Identifier::Type::Double)
                std::wcout << identifier.value.doubleValue;
            else
                std::wcout << L"";
        }
        else
            std::wcout << L"";

        std::wcout << std::setw(10) << Identifier::typeToString(identifier.returnType) << std::setw(5)
                   << m_tokens[static_cast<size_t>(identifier.tokenIndex)].line << std::setw(5)
                   << m_tokens[static_cast<size_t>(identifier.tokenIndex)].position - static_cast<int>(identifier.name.size())
                   << std::setw(80) <<  m_files[static_cast<size_t>(m_tokens[static_cast<size_t>(identifier.tokenIndex)].fileIndex)] << std::endl;
    }
    std::wcout << std::setfill(L'=') << std::setw(190) << L' ' << std::setfill(L' ') << std::endl;
}

int Tokenizer::findDeclaration(const std::wstring &wsrc)
{
    for(int i = static_cast<int>(m_identifiers.size() - 1) ; i>= 0; --i)
        if ( (m_identifiers[static_cast<size_t>(i)].context == Identifier::Context::Declaration || m_identifiers[static_cast<size_t>(i)].context == Identifier::Context::Argument)
             && m_identifiers[static_cast<size_t>(i)].decoratedName == wsrc)
            return i;
    return -1;
}

const std::vector<Identifier> &Tokenizer::identifiers() const
{
    return m_identifiers;
}

const std::vector<Token> &Tokenizer::tokens() const
{
    return m_tokens;
}

const std::vector<std::wstring> &Tokenizer::files() const
{
    return m_files;
}

std::vector<std::wstring> &Tokenizer::files()
{
    return m_files;
}

Identifier::Identifier() : rbpOffset(0), linkTo(-1)
{
    type = Type::Undefined;
    returnType = Type::Undefined;
    context = Context::Default;

}

Identifier Identifier::createIntegerLiteral(const int value)
{
    Identifier id;
    id.type = Identifier::Type::Integer;
    id.value.intValue = value;
    return id;
}

std::wstring Identifier::contextToString(const Identifier::Context cnt)
{
    switch(cnt)
    {
    case Context::Default:
        return L"Default";
    case Context::Argument:
        return L"Argument";
    case Context::Link:
        return L"Link";
    case Context::Literal:
        return L"Literal";
    case Context::Declaration:
        return L"Declaration";
    }
    return L"Default";
}

Identifier::Type Identifier::typeFromWChar(const wchar_t ch)
{
    switch(ch)
    {
    case L's':
        return Type::String;
    case L'd':
        return Type::Double;
    case L'i':
        return Type::Integer;
    case L'f':
        return Type::Function;
    case L'b':
        return Type::Bool;
    }
    return Type::String;
}

wchar_t Identifier::typeToWChar(const Identifier::Type tp)
{
    switch(tp)
    {
    case Type::Bool:
        return L'b';
    case Type::Double:
        return L'd';
    case Type::Function:
        return L'f';
    case Type::Integer:
        return L'i';
    case Type::String:
        return L's';
    }
    return L'#';
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
    case Type::Undefined:
        return L"#";
    }
    return std::wstring();
}

std::wstring Identifier::typeToAsm(const Identifier::Type tp)
{
    switch(tp)
    {
    case Type::Integer:
        return L".int";
    }
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
    case Type::Undefined:
        return 0;
    case Type::Integer:
        return 4;
    case Type::Bool:
        return 1;
    case Type::String:
        return 256;
    case Type::Double:
        return 8;
    case Type::Function:
        return 0;
    }
    return 0;
}

std::wstring Token::vectorToWString(const std::vector<Token> &src)
{
    std::wstring result;
    for (const Token &token : src)
        result += token.token;
    return result;
}

int Token::getNextIdentifierIdx(const std::vector<Token> &src, const int offset)
{
    for(std::vector<Token>::const_iterator it = src.cbegin() + offset; it != src.end(); ++it)
    {
        if (it->identifierIdx != -1)
            return static_cast<int>(std::distance(src.begin(), it));
    }
    return -1;
}

} // namespace Transducer
} // namespace PDA
