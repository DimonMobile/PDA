#include "preprocessor.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include "../Utils/settings.h"
#include "../Exception/not_found_exception.h"
#include "../Exception/source_exception.h"

namespace PDA
{
namespace Transducer
{

namespace Constants
{
    const std::wstring scannerType = L"preprocessor";
    const std::wstring quotesExceptionString = L"detected, but filename not found";
    const std::wstring includeDirective = L"@уключыць";
    const wchar_t quoteChar = L'\"';
    const wchar_t directiveChar = L'@';
}

Preprocessor::Preprocessor(const std::string &fileName) : m_lineIndex(0)
{
    m_currentFileName = fileName;
    std::wifstream inputFile(m_currentFileName);
    if (inputFile.is_open())
    {
        while(inputFile)
        {
            std::wstring line;
            std::getline(inputFile, line);
            m_source.append(line);
            m_source.append(L"\r\n");
        }
    }
    else
    {
        throw PDA::Exception::NotFoundException(std::wstring(fileName.begin(), fileName.end()));
    }
}

void Preprocessor::execute()
{
    std::wstring result;
    std::wstringstream stream(m_source);
    m_haveChanges = false;
    while(stream)
    {
        std::wstring line;
        std::getline(stream, line);
        ++m_lineIndex;
        if (!line.empty() && line[0] == Constants::directiveChar && line.find(Constants::includeDirective) == 0)
        {
            size_t firstQuotePos = line.find_first_of(Constants::quoteChar);
            size_t lastQuotePos = line.find_last_of(Constants::quoteChar);
            if (firstQuotePos == lastQuotePos || firstQuotePos == std::wstring::npos || lastQuotePos == std::wstring::npos)
            {
                throw PDA::Exception::SourceException(static_cast<int>(m_lineIndex), 0, Constants::scannerType + L':' + Constants::includeDirective + L' ' + Constants::quotesExceptionString);
            }
            else
            {
                std::wstring wFileName = line.substr(firstQuotePos + 1, lastQuotePos - firstQuotePos - 1);
                std::wstring includeFilePath = PDA::Utils::Settings::Instance().includeFilePath();
                if (!includeFilePath.empty())
                {
                    wFileName = includeFilePath + L'/' + wFileName;
                }
                std::string fileName(wFileName.begin(), wFileName.end());
                result.append(L"@нумар " + std::to_wstring(m_lineIndex) + L' ' + wFileName + L'\n');
                try
                {
                    Preprocessor innerPreproc(fileName);
                    innerPreproc.execute();
                    result.append(innerPreproc.m_source);
                    result.append(L"@нумар " + std::to_wstring(m_lineIndex) +  L'\n');
                }
                catch (const Exception::NotFoundException &e)
                {
                    throw PDA::Exception::SourceException(m_lineIndex, 0, std::wstring(m_currentFileName.begin(), m_currentFileName.end()) + L" missing " + e.what());
                }

            }
            m_haveChanges = true;
        }
        else
            result.append(line + L'\n');
    }
    m_source = result;
}

const std::wstring &Preprocessor::source() const
{
    return m_source;
}

} // namespace Transducer
} // namespace PDA
