#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>

namespace PDA
{
namespace Exception
{

class Exception
{
public:
    Exception(const std::wstring &what = std::wstring());
    std::wstring what() const;
private:
    std::wstring m_information;
};

} // namespace Exception
} // namespace PDA

#endif // EXCEPTION_H
