#include "misc.h"

#include <vector>
#include <exception>

namespace PDA
{
namespace Utils
{
std::wstring Misc::toBase64(unsigned char const* buffer, size_t size){
    using std::wstring;
    static wchar_t const* base64Table =
        L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    size_t base64Size = (size + 2 - ((size + 2) % 3)) / 3 * 4;
    wstring result(base64Size, L'A');

    unsigned char const* s = buffer;  // source pointer
    size_t di = 0;                    // destination index
    for(size_t i = 0; i < size / 3; i++){
        // input: 0000_0000 0000_0000 0000_0000
        //
        // out1:  0000_00
        // out2:         00 0000
        // out3:                _0000 00
        // out4:                        00_0000

        result[di++] = base64Table[s[0] >> 2];
        result[di++] = base64Table[((s[0] << 4) | (s[1] >> 4)) & 0x3f];
        result[di++] = base64Table[((s[1] << 2) | (s[2] >> 6)) & 0x3f];
        result[di++] = base64Table[s[2] & 0x3f];
        s += 3;
    }

    size_t remainSize = size % 3;
    switch(remainSize){
    case 0:
        break;
    case 1:
        result[di++] = base64Table[s[0] >> 2];
        result[di++] = base64Table[(s[0] << 4) & 0x3f];
        break;
    case 2:
        result[di++] = base64Table[s[0] >> 2];
        result[di++] = base64Table[((s[0] << 4) | (s[1] >> 4)) & 0x3f];
        result[di++] = base64Table[(s[1] << 2) & 0x3f];
        break;
    }
    return result;
}

} // namespace Utils
} // namespace PDA
