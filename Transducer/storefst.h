#ifndef MFST_H
#define MFST_H

#include <string>
#include <stack>
#include <vector>
#include <cwctype>
#include <iostream>
#include <iomanip>

#include "tokenizer.h"
#include "Exception/source_exception.h"
#include "Utils/settings.h"

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

        Rule &setErrorString(const std::wstring &info)
        {
            m_errorString = info;
            return *this;
        }

        wchar_t m_left;
        std::vector<Chain> m_chains;
        std::wstring m_errorString;
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
        State(const wchar_t startSymbol = L'S') : m_ruleIndex(-1), m_chainIndex(-1), m_sourcePosition(0)
        {
            m_stack.push_back(startSymbol);
        }
    };
    void setGrammar(const Grammar &grammar)
    {
        m_grammar = grammar;
    }

    void setSource(Tokenizer &tokenizer)
    {
        m_currentState = State(m_grammar.startSymbol());
        m_tokens = tokenizer.tokens();
        m_files = &tokenizer.files();
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
                    if (m_currentState.m_ruleIndex == -1)
                    {
                        m_currentState.m_ruleIndex = findRule(m_currentState.m_stack.back());
                        m_currentState.m_chainIndex = -1;
                    }
                    if (m_currentState.m_ruleIndex == -1)
                    {
                        if (Utils::Settings::Instance().isSyntaxTraceEnabled())
                            std::wcerr << std::setw(10) << L"Error: no rule found(" << m_currentState.m_stack.back() << L')' << std::endl;
                        raise(L"Grammar error, no rule");
                    }
                    if (Utils::Settings::Instance().isSyntaxTraceEnabled())
                        std::wcout << std::setw(10) << L"NT " << m_currentState.m_stack.back() << L"[" << m_currentState.m_chainIndex + 1 << L"]-> "
                                   << ((m_currentState.m_chainIndex + 1 < static_cast<int>(m_grammar.rules()[static_cast<size_t>(m_currentState.m_ruleIndex)].m_chains.size())) ?
                                    m_grammar.rules()[static_cast<size_t>(m_currentState.m_ruleIndex)].m_chains[static_cast<size_t>(m_currentState.m_chainIndex + 1)].m_chain : L"<EMPTY>") << std::endl;

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
                        rollback();
                    }
                }
            }
            else // Store empty
            {

                rollback();
            }
        }
        else // End of source
        {
            if (Utils::Settings::Instance().isSyntaxTraceEnabled())
                std::wcout << std::setw(10) << L"Source empty" << std::endl;
            if (m_currentState.m_stack.empty())
            {
                if (Utils::Settings::Instance().isSyntaxTraceEnabled())
                    std::wcout << std::setw(10) << L"Parsing complete" << std::endl;
                return false;
            }
            else
            {
                if (m_states.empty())
                    raise(L"mismatch");
                else
                    rollback();
            }
        }
        debugLine();
        return true;
    }
    void printRulesSequence()
    {
        while(!m_states.empty())
        {
            std::wcout << m_grammar.rules()[m_states.top().m_ruleIndex].m_left << L" - > " << m_grammar.rules()[m_states.top().m_ruleIndex].m_chains[m_states.top().m_chainIndex + 1].m_chain  << std::endl;
            m_states.pop();
        }
    }
    void debugLine()
    {
        if (Utils::Settings::Instance().isSyntaxTraceEnabled())
            std::wcout << std::right << std::setw(30) << debugSource() << std::setw(30) <<  debugStack() << std::endl;
    }
    StoreFst(const std::vector<Token> &tokens) : m_tokens(tokens)
    { }
    StoreFst(const Grammar &grammar) : m_grammar(grammar)
    { }
private:
    [[noreturn]]
    void raise(const std::wstring &info)
    {
        Token &token = m_tokens[static_cast<size_t>(m_mostState.m_sourcePosition)];
        if (m_mostState.m_ruleIndex >= 0)
            throw Exception::SourceException(token.line, token.position - 1, L"\"" + (*m_files)[static_cast<size_t>(token.fileIndex)] + L"\"" + L" " + m_grammar.rules()[static_cast<size_t>(m_mostState.m_ruleIndex)].m_errorString);
        else
            throw Exception::SourceException(token.line, token.position - 1, L"\"" + (*m_files)[static_cast<size_t>(token.fileIndex)] + L"\"" + L" " + info);
    }

    bool rollback()
    {
        if (Utils::Settings::Instance().isSyntaxTraceEnabled())
            std::wcout << std::setw(10) << L"Rollback" << std::endl;
        if (m_currentState.m_sourcePosition >= m_mostState.m_sourcePosition)
            m_mostState = m_currentState;
        do
        {
            if (m_states.empty())
                raise(L"Stack empty");
            m_currentState = m_states.top();
            m_states.pop();
            ++m_currentState.m_chainIndex;
        } while (m_currentState.m_chainIndex >= static_cast<int>(m_grammar.rules()[static_cast<size_t>(m_currentState.m_ruleIndex)].m_chains.size()));
        return true;
    }

    void expandCurrentNTerminal()
    {
        m_currentState.m_stack.pop_back();
        ++m_currentState.m_chainIndex;
        if ( m_currentState.m_chainIndex >= static_cast<int>(m_grammar.rules()[static_cast<size_t>(m_currentState.m_ruleIndex)].m_chains.size()))
        {
            rollback();
            return;
        }
        std::wstring currentChain = m_grammar.rules()[static_cast<size_t>(m_currentState.m_ruleIndex)].m_chains[static_cast<size_t>(m_currentState.m_chainIndex)].m_chain;
        for(auto it = currentChain.crbegin(); it != currentChain.crend(); ++it)
            m_currentState.m_stack.push_back(*it);
        m_currentState.m_ruleIndex = -1;
    }

    int findRule(const wchar_t nt)
    {
        if ( m_currentState.m_ruleIndex >= 0 && m_currentState.m_ruleIndex < static_cast<int>(m_grammar.rules().size()) && m_grammar.rules()[static_cast<size_t>(m_currentState.m_ruleIndex)].m_left == nt)
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
    State m_mostState;
    Grammar m_grammar;
    std::vector<Token> m_tokens;
    std::vector<std::wstring> *m_files;
    std::stack<State> m_states;
};

} // namespace Transducer
} // namespace PDA

#endif // MFST_H
