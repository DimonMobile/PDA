#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <list>
#include <vector>
#include "fst.h"

namespace PDA
{
namespace Transducer
{

struct Token
{
    wchar_t token;
    int position, line;
    int fileIndex;
};

struct Identifier
{
    std::wstring name;
    enum class Type
    {
        Integer,
        Bool,
        String,
        Double,
        Function
    } type, returnType;
    int tokenIndex;
    static std::wstring typeToString(const Type tp);

    static Type typeFromString(const std::wstring &tp);
    int size();
};

class Tokenizer
{
public:
    Tokenizer(const std::wstring &source);
    void printTokens();
    void printIdentifiers();
private:
    void commitToken();
private:
    bool m_willFunction, m_isPrevTokenTypeToken;
    int m_lastFunctionTokenIndex;
    int m_currentPos, m_currentLine;
    std::wstring m_currentFile;
    std::wstring m_token;
    std::vector<Token> m_tokens;
    std::vector<Identifier> m_identifiers;
    std::vector<std::wstring> m_files;
    Fst m_fst;
};


} // namespace Transducer
} // namespace PDA
#endif // TOKENIZER_H
