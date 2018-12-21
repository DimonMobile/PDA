#ifndef PARAMS_H
#define PARAMS_H

#include <string>

#define PARAM_BUFFER_SIZE   1024

namespace PDA
{
namespace Utils
{


class Settings
{
public:
    static Settings &Instance();
    void initParams(int argc, char** argv);

    std::string sourceFilePath() const;
    std::wstring includeFilePath() const;
    std::wstring outputFilePath() const;

    bool isSyntaxTraceEnabled() const;

    Settings(const Settings&) = delete;
    void operator = (const Settings&) = delete;

private:
    Settings();
    std::wstring m_logFilePath;
    std::wstring m_sourceFilePath;
    std::wstring m_includeFilePath;
    std::wstring m_outputFilePath;
    bool m_syntaxTraceEnable;
    bool m_isVersion;
    bool m_isHelp;
};


} // namespace Utils
} // namespace PDA

#endif // PARAMS_H
