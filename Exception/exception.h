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
    virtual std::wstring what() const;
protected:
    std::wstring m_information;
};

} // namespace Exception
} // namespace PDA

#endif // EXCEPTION_H
