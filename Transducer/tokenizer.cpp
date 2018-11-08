#include "tokenizer.h"

#include <cwctype>
#include <iostream>
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
    std::wstring readedLineIndex, readedFileName;
    std::string currentFileS(PDA::Utils::Settings::Instance().sourceFilePath());
    std::wstring currentFile(currentFileS.begin(), currentFileS.end());
    int currentPos{0}, currentLine{1};
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
                    readedFileName += currentChar;
                }
                else
                {
                    preprocReadingStatus = 0;
                    currentFile = readedFileName;
                    currentPos = 0;
                    currentLine = std::stoi(readedLineIndex);

                    readedFileName.clear();
                    readedLineIndex.clear();
                    isPreprocLine = false;
                }
            }
        }

        if (isPreprocLine)
            continue;

        ++currentPos;
        if (currentChar == '\n')
        {
            ++currentLine;
            currentPos = 0;
            isStartLine = true;
        }


        if (quoteStarted && currentChar != '\"')
        {
            if (currentChar == '\n') // newline on quote
            {
                throw Exception::SourceException(currentLine, currentPos, currentFile + L" didn't closed \"(did you forge to close your string literal?)");
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
        else if (currentChar == ':' || currentChar == ';')
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
    std::wcout << m_token << std::endl;
    if (!m_fst.execute(m_token))
        std::wcout << L"***\t***Не распознан" << std::endl;
    m_token.clear();
}

} // namespace Transducer
} // namespace PDA
