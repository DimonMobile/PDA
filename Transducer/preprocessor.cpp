#include "preprocessor.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include "../Utils/settings.h"
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
    std::wifstream inputFile(fileName);
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
        throw PDA::Exception::Exception(L"source file not found");
    }
}

void Preprocessor::execute()
{
    std::wstring result;
    std::wstringstream stream(m_source);
    while(stream)
    {
        std::wstring line;
        std::getline(stream, line);
        ++m_lineIndex;
        if (!line.empty() && line[0] == Constants::directiveChar && line.find_first_of(Constants::includeDirective) == 0)
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
                std::wifstream subFile(fileName);
                if (!subFile.is_open())
                    throw PDA::Exception::SourceException(static_cast<int>(m_lineIndex), 0, Constants::scannerType + L':' + wFileName + L" not found.");
                while(subFile)
                {
                    std::wstring currentLine;
                    std::getline(subFile, currentLine);
                    result.append(currentLine + L'\n');
                }
                result.append(L"@нумар " + std::to_wstring(m_lineIndex) + L'\n');
            }
        }
        else
            result.append(line + L'\n');
    }
    std::wcout << result << std::endl;
}

} // namespace Transducer
} // namespace PDA
