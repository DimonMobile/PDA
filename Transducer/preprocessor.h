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
    void execute();
private:
    std::wstring m_source;
    size_t m_lineIndex;
};
} // namespace Transducer
} // namespace PDA
#endif // PREPROCESSOR_H
