#ifndef MFST_H
#define MFST_H

#include <string>
#include <stack>
#include <vector>
#include <cwctype>
#include <iostream>
#include <iomanip>

#include "tokenizer.h"

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
    Grammar(const wchar_t startSymbol, const First &first, const Rest&... rest) : m_startSymbol(startSymbol)
    {
        addRules(first, rest...);
    }
    Grammar()
    {

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

    wchar_t startSymbol()
    {
        return m_startSymbol;
    }
    std::vector<Rule> &rules()
    {
        return m_rules;
    }
private:
    std::vector<Rule> m_rules;
    wchar_t m_startSymbol;
};

class StoreFst
{
public:
    struct State
    {
        int m_ruleIndex;
        int m_chainIndex;
        std::vector<wchar_t> m_stack;
        int m_sourcePosition;
        State(const wchar_t startSymbol = L'S') : m_ruleIndex(0), m_chainIndex(0), m_sourcePosition(0)
        {
            m_stack.push_back(startSymbol);
        }
    };
    void setGrammar(const Grammar &grammar)
    {
        m_grammar = grammar;
    }

    void setSource(const std::vector<Token> &tokens)
    {
        m_currentState = State(m_grammar.startSymbol());
        m_tokens = tokens;
        while(!m_states.empty())
            m_states.pop();
    }

    void step()
    {
        std::wcout << std::setw(10) << L"Before";
        std::wcout << std::setw(30) << debugSource() << std::setw(30) << debugStack() << std::endl;

        if (m_currentState.m_sourcePosition < static_cast<int>(m_tokens.size()))
        {
            if (!m_currentState.m_stack.empty())
            {

            }
            else // Store empty
            {

            }
        }
        else // End of source
        {

        }

        std::wcout << std::setw(10) << L"After";
        std::wcout << std::setw(30) << debugSource() << std::setw(30) << debugStack() << std::endl;
    }
    StoreFst(const std::vector<Token> &tokens) : m_tokens(tokens)
    { }
    StoreFst(const Grammar &grammar) : m_grammar(grammar)
    { }
private:
    std::wstring debugSource()
    {
        std::wstring result;
        for(int i = m_currentState.m_sourcePosition; i <= m_currentState.m_sourcePosition + 5 && m_currentState.m_sourcePosition < static_cast<int>(m_tokens.size()); ++i )
        {
            result += m_tokens[static_cast<size_t>(i)].token;
        }
        return result;
    }

    std::wstring debugStack()
    {
        std::wstring result;
        int n = static_cast<int>(m_currentState.m_stack.size()) - 5;
        for(int i = static_cast<int>(m_currentState.m_stack.size()) - 1; i >= n && i >= 0; --i)
        {
            result += m_currentState.m_stack[static_cast<size_t>(i)];
        }
        return result;
    }
private:
    State m_currentState;
    Grammar m_grammar;
    std::vector<Token> m_tokens;
    std::stack<State> m_states;
};

} // namespace Transducer
} // namespace PDA

#endif // MFST_H
