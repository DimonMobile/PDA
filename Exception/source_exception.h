#ifndef SOURCE_EXCEPTION_H
#define SOURCE_EXCEPTION_H

#include "exception.h"

namespace PDA
{
namespace Exception
{

class SourceException : public Exception
{
public:
    SourceException(int line = -1, int position = -1, const std::wstring &what = L"");
    std::wstring what() const override;
private:
    int m_line;
    int m_position;
};

} // namespace Exception
} // namespace PDA

#endif // SOURCE_EXCEPTION_H
