#include "source_exception.h"

namespace PDA
{
namespace Exception
{

SourceException::SourceException(int line, int position, const std::wstring &what) : Exception(what)
  , m_line(line)
  , m_position(position)
{ }

std::wstring SourceException::what() const
{
    return std::to_wstring(m_line) + L':' + std::to_wstring(m_position) + L':' + m_information;
}

} // namespace Exception
} // namespace PDA
