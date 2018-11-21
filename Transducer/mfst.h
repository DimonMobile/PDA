#ifndef MFST_H
#define MFST_H

#include <string>
#include <stack>
#include <vector>

namespace PDA
{
namespace Transducer
{

class Grammar
{
public:

    struct Chain
    {
        Chain(const std::wstring &src) : m_chain(src)
        { }
        std::wstring m_chain;
    };

    struct Rule
    {
        template<typename First, typename ...Rest>
        Rule(const wchar_t leftSide, const First &first, const Rest&... rest)
        {
            m_left = leftSide;
            addChains(first, rest...);
        }
        template<typename First, typename ...Rest>
        void addChains(const First &first, const Rest&... rest)
        {
            m_chains.push_back(Chain(first));
            addChains(rest...);
        }
        template<typename T>
        void addChains(const T &t)
        {
            m_chains.push_back(Chain(t));
        }

        wchar_t m_left;
        std::vector<Chain> m_chains;
    };

    template<typename First, typename ...Rest>
    Grammar(const First &first, const Rest&... rest)
    {
        addRules(first, rest...);
    }

    template<typename First, typename ...Rest>
    void addRules(const First &first, const Rest&... rest)
    {
        m_rules.push_back(first);
        addRules(rest...);
    }

    void addRules(const Rule& rule)
    {
        m_rules.push_back(rule);
    }
private:
    std::vector<Rule> m_rules;
};

class Mfst
{
public:
private:
    std::wstring m_source;
};

} // namespace Transducer
} // namespace PDA

#endif // MFST_H
