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
    std::wstring decoratedName;
    enum class Type
    {
        Undefined,
        Integer,
        Bool,
        String,
        Double,
        Function
    } type, returnType;
    enum class Context
    {
        Default,
        Declaration,
        Link,
        Argument,
        Literal
    } context;

    struct IdentifierValue
    {
        union
        {
            long long intValue; // keeps string length in case of string type [Format: <8 bytes length><string data...>]
            double doubleValue;
        };
        std::wstring stringValue;
    } value;

    int rbpOffset;
    int linkTo;
    int tokenIndex;
    std::vector<Type> functionArgTypes;
    Identifier();
    static std::wstring contextToString(const Context cnt);
    static Type typeFromWChar(const wchar_t ch);
    static wchar_t typeToWChar(const Type tp);
    static std::wstring typeToString(const Type tp);
    static std::wstring typeToAsm(const Type tp);
    static Type typeFromString(const std::wstring &tp);
    int size();

};

class Tokenizer
{
public:
    Tokenizer(const std::wstring &source);
    void printTokens() const;
    void printIdentifiers() const;
    int findDeclaration(const std::wstring &wsrc);
    const std::vector<Identifier> &identifiers() const;
    const std::vector<Token> &tokens() const;
    const std::vector<std::wstring> &files() const;
    std::vector<std::wstring> &files();
private:
    void commitToken();
private:
    bool m_willFunction, m_isPrevTokenTypeToken, m_isFunctionArgs;
    std::wstring m_willFunctionName;
    int m_lastFunctionIdentifierIndex;
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
