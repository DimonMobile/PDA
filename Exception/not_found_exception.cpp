#include "not_found_exception.h"

namespace PDA
{
namespace Exception
{

NotFoundException::NotFoundException(const std::wstring &what) : Exception(what)
{ }

std::wstring NotFoundException::what() const
{
    return m_information;
}

} // namespace Exception
} // namespace PDA
