#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <string>

class Preprocessor
{
public:
    Preprocessor(const std::wstring &src);
private:
    const std::wstring &m_source;
};

#endif // PREPROCESSOR_H
