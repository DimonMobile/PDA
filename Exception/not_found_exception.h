#ifndef NOT_FOUND_EXCEPTION
#define NOT_FOUND_EXCEPTION

#include "exception.h"

namespace PDA
{
namespace Exception
{

class NotFoundException : public Exception
{
public:
    NotFoundException(const std::wstring &what = L"");
    std::wstring what() const override;
};

} // namespace Exception
} // namespace PDA

#endif // SOURCE_EXCEPTION_H
