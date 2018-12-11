#ifndef MISC_H
#define MISC_H

#include <string>

namespace PDA
{
namespace Utils
{
class Misc
{
public:
    Misc() = delete;
    static std::wstring toBase64(unsigned char const* buffer, size_t size);
};

} // namespace Utils
} // namespace PDA
#endif // MISC_H
