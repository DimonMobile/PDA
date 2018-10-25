#ifndef PARAMS_EXCEPTION_H
#define PARAMS_EXCEPTION_H

#include "exception.h"

namespace PDA
{
namespace Exception
{

class ParamsException : public Exception
{
public:
    ParamsException(const std::wstring &what);
};

} // namespace PDA
} // namespace Exception
#endif // PARAMS_EXCEPTION_H
