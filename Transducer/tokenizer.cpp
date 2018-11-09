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

Tokenizer::Tokenizer(const std::wstring &source)
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
    /*else // TODO: token debug flag show text below
        std::wcout << std::setw(3) << m_currentLine << L':' << std::setw(3) << m_currentPos << L':' << std::setw(100) << std::wstring(m_currentFile.begin(), m_currentFile.end()) << L">" << std::setw(50) << m_token << L" -> " <<  Fst::userData << std::endl;*/
    m_token.clear();
}

} // namespace Transducer
} // namespace PDA
