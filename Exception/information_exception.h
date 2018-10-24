#ifndef INFORMATIONEXCEPTION_H
#define INFORMATIONEXCEPTION_H

#include "exception.h"

namespace PDA
{
namespace Exception
{

class InformationException : public Exception
{
public:
    InformationException(const std::wstring &what);
};


} // namespace Exception
} // namespace PDA

#endif // INFORMATIONEXCEPTION_H
