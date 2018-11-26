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

    bool step()
    {
        if (m_currentState.m_sourcePosition < static_cast<int>(m_tokens.size()))
        {
            if (!m_currentState.m_stack.empty())
            {
                if (!isTerminalOnTop())
                {
                    m_currentState.m_ruleIndex = findRule(m_currentState.m_stack.back());
                    if (m_currentState.m_ruleIndex == -1)
                    {
                        std::wcerr << std::setw(10) << "Error: no rule found(" << m_currentState.m_stack.back() << std::endl;
                        return false;
                    }
                    std::wcout << std::setw(10) << "Expand NT " << m_currentState.m_stack.back() << " with chain[" << m_currentState.m_chainIndex << "]: "
                               << ((m_currentState.m_chainIndex < static_cast<int>(m_grammar.rules()[static_cast<size_t>(m_currentState.m_ruleIndex)].m_chains.size())) ?
                                m_grammar.rules()[static_cast<size_t>(m_currentState.m_ruleIndex)].m_chains[static_cast<size_t>(m_currentState.m_chainIndex)].m_chain : L"<EMPTY>") << std::endl;

                    m_states.push(m_currentState);
                    expandCurrentNTerminal();
                }
                else
                {
                    if (m_currentState.m_stack.back() == m_tokens[static_cast<size_t>(m_currentState.m_sourcePosition)].token)
                    {
                        ++m_currentState.m_sourcePosition;
                        m_currentState.m_stack.pop_back();
                    }
                    else
                    {
                        std::wcout << std::setw(10) << "Stack and source mismatch -> rollback and trying next chain..." << std::endl;
                        if (m_states.empty())
                        {
                            std::wcerr << std::setw(10) << "Error: stack empty" << std::endl;
                            return false;
                        }
                        rollback();
                    }
                }
            }
            else // Store empty
            {
                std::wcout << std::setw(10) << "Store empty -> rollback and trying next chain..." << std::endl;
                if (m_states.empty())
                {
                    std::wcerr << std::setw(10) << "Error: stack empty" << std::endl;
                    return false;
                }
                rollback();
            }
        }
        else // End of source
        {
            std::wcout << std::setw(10) << "Source empty" << std::endl;
            return false;
        }
        debugLine();
        return true;
    }
    void debugLine()
    {
        std::wcout << '[' << std::setw(30) << std::right << debugSource() << "] [" << std::setw(30) << std::left << debugStack() << ']' << std::endl;
    }
    StoreFst(const std::vector<Token> &tokens) : m_tokens(tokens)
    { }
    StoreFst(const Grammar &grammar) : m_grammar(grammar)
    { }
private:
    bool rollback()
    {
        do
        {
            if (m_states.empty())
                return false;
            m_currentState = m_states.top();
            m_states.pop();
            ++m_currentState.m_chainIndex;
        } while (m_currentState.m_chainIndex >= static_cast<int>(m_grammar.rules()[static_cast<size_t>(m_currentState.m_ruleIndex)].m_chains.size()));
        return true;
    }

    void expandCurrentNTerminal()
    {
        m_currentState.m_stack.pop_back();
        if ( m_currentState.m_chainIndex >= static_cast<int>(m_grammar.rules()[static_cast<size_t>(m_currentState.m_ruleIndex)].m_chains.size()))
        {
            rollback();
            return;
        }
        std::wstring currentChain = m_grammar.rules()[static_cast<size_t>(m_currentState.m_ruleIndex)].m_chains[static_cast<size_t>(m_currentState.m_chainIndex)].m_chain;
        for(auto it = currentChain.crbegin(); it != currentChain.crend(); ++it)
            m_currentState.m_stack.push_back(*it);
    }

    int findRule(const wchar_t nt)
    {
        if (m_grammar.rules()[static_cast<size_t>(m_currentState.m_ruleIndex)].m_left == nt)
            return m_currentState.m_ruleIndex;
        for(size_t i = 0 ; i < m_grammar.rules().size(); ++i)
            if (m_grammar.rules()[i].m_left == nt)
                return static_cast<int>(i);
        return -1;
    }

    bool isTerminalOnTop()
    {
        if ( iswupper(static_cast<wint_t>(m_currentState.m_stack.back())) )
            return false;
        else
            return true;
    }

    std::wstring debugSource()
    {
        std::wstring result;
        for(int i = m_currentState.m_sourcePosition; (i <= m_currentState.m_sourcePosition + 25) && (i < static_cast<int>(m_tokens.size())); ++i )
        {
            result += m_tokens[static_cast<size_t>(i)].token;
        }
        return result;
    }

    std::wstring debugStack()
    {
        std::wstring result;
        int n = static_cast<int>(m_currentState.m_stack.size()) - 25;
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
