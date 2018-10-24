#include "exception.h"

namespace PDA
{
namespace Exception
{


Exception::Exception(const std::wstring &what) : m_information(what)
{

}

std::wstring Exception::what() const
{
    return m_information;
}


} // namespace Exception
} // namespace PDA
