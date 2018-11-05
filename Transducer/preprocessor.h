#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <string>

namespace PDA
{
namespace Transducer
{
class Preprocessor
{
public:
    Preprocessor(const std::string &fileName);
    const std::wstring &source() const;
private:
    void execute();
private:
    std::wstring m_source;
    std::string m_currentFileName;
    size_t m_lineIndex;
    bool m_haveChanges;
};
} // namespace Transducer
} // namespace PDA
#endif // PREPROCESSOR_H
