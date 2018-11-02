#include "settings.h"

#include "../Exception/information_exception.h"
#include "../Exception/params_exception.h"

namespace PDA
{
namespace Utils
{


namespace Constants
{
    const std::wstring organizationName = L"(c) Plotnikov Dmitry";
    const std::wstring applicationName = L"PDA-2018";
    const std::wstring applicationDisplayName = L"PDA-2018 - Plotnikov Dmitry Andreevich Complier 2018";
    const std::wstring applicationVersion = L"1.0 Alpha";

    const std::wstring helpArgumentLong = L"--help";
    const std::wstring helpArgumentShort = L"-h";
    const std::wstring versionArgumentLong = L"--version";
    const std::wstring versionArgumentShort = L"-V";
    const std::wstring sourceFilePathArgumentLong = L"--source";
    const std::wstring sourceFilePathArgumentShort = L"-S";
    const std::wstring logFilePathArgumentLong = L"--log";
    const std::wstring logFilePathArgumentShort = L"-L";
    const std::wstring includePathArgumentLong = L"--include";
    const std::wstring includePathArgumentShort = L"-I";
    const std::wstring helpText = applicationDisplayName + L' ' + applicationVersion + L"\n\n"
                                  L"Arguments:\n"
                                  L"   -h or --help\t\t\tPrint help(this message) and exit\n"
                                  L"   -V or --version\t\tPrint version information and exit\n"
                                  L"   -S or --source\t\tSet source file\n"
                                  L"   -I or --include\t\tSet directory where includes will be found\n";
} // namespace Constants


void Settings::initParams(int argc, char **argv)
{
    m_isHelp = false;
    m_isVersion = false;

    for(int i = 1 ; i < argc; ++i)
    {
        wchar_t buf[PARAM_BUFFER_SIZE + 1];
        std::wstring currentParam;
        mbstowcs(buf, argv[i], PARAM_BUFFER_SIZE);
        currentParam = buf;
        if (currentParam == Constants::helpArgumentLong || currentParam == Constants::helpArgumentShort)
        {
            m_isHelp = true;
        }
        else if (currentParam == Constants::versionArgumentLong || currentParam == Constants::versionArgumentShort)
        {
            m_isVersion = true;
        }
        else if (currentParam == Constants::sourceFilePathArgumentLong || currentParam == Constants::sourceFilePathArgumentShort)
        {
            if (++i < argc)
            {
                mbstowcs(buf, argv[i], PARAM_BUFFER_SIZE);
                m_sourceFilePath = buf;
            }
            else
            {
                throw Exception::ParamsException( L"Key -S (--source) used but without argument" );
            }
        }
        else if (currentParam == Constants::includePathArgumentLong || currentParam == Constants::includePathArgumentShort)
        {
            if (++i < argc)
            {
                mbstowcs(buf, argv[i], PARAM_BUFFER_SIZE);
                m_includeFilePath = buf;
            }
            else
            {
                throw Exception::ParamsException( L"Key -I (--include) used but without argument" );
            }
        }
        else if (currentParam == Constants::logFilePathArgumentLong || currentParam == Constants::logFilePathArgumentShort)
        {

        }
    }
    if (m_isHelp)
    {
        throw Exception::InformationException(Constants::helpText);
    }
    else if (m_isVersion)
    {
        throw Exception::InformationException(Constants::applicationName + L' ' + Constants::applicationVersion + L'\n' + Constants::organizationName);
    }
    else if (m_sourceFilePath.empty())
    {
        throw Exception::ParamsException( L"No source file" );
    }
}

std::string Settings::sourceFilePath() const
{
    return std::string(m_sourceFilePath.begin(), m_sourceFilePath.end());
}

Settings::Settings()
{

}

std::wstring Settings::includeFilePath() const
{
    return m_includeFilePath;
}

Settings &Settings::Instance()
{
    static Settings instance;
    return instance;
}

} // namespace Utils
} // namesapce PDA