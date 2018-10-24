#ifndef FST_H
#define FST_H

#include <functional>
#include <vector>
#include <stack>
#include <list>
#include <set>

#define DEFAULT_FST_UNIT wchar_t

namespace PDA
{

namespace Transducer
{

template<typename T = DEFAULT_FST_UNIT>
struct AbstractEdge
{
    std::set<T> m_transitions;

    AbstractEdge()
    {
    }

    AbstractEdge(const std::initializer_list<T> &initList)
    {
        for(const T &item : initList)
        {
            m_transitions.insert(item);
        }
    }

    AbstractEdge(const AbstractEdge<T> &from)
    {
        m_transitions = from.m_transitions;
    }
    /// Move semantics constructor
    AbstractEdge(AbstractEdge<T> &&from)
    {
        m_transitions = std::move(from.m_transitions);
    }
    /// Move semantics assignment operator
    AbstractEdge<T>& operator = (AbstractEdge<T> &&from)
    {
        m_transitions = std::move(from.m_transitions);
    }
    AbstractEdge<T>& operator = (const AbstractEdge<T> &from)
    {
        m_transitions = from.m_transitions;
    }

    template<typename ...Rest>
    static AbstractEdge<T> create(T first, Rest ...rest)
    {
        AbstractEdge<T> result = create(rest...);
        result.m_transitions.insert(first);
        return result;
    }

    static AbstractEdge<T> create(T first)
    {
        AbstractEdge<T> result;
        result.m_transitions.insert(first);
        return result;
    }
};
typedef AbstractEdge<> Edge;


template<typename T = DEFAULT_FST_UNIT>
class AbstractFst
{
public:
    typedef std::pair<int, AbstractEdge<T>> Transition;
    struct Vertex
    {
        Vertex() {}

        template<typename First, typename ...Rest>
        Vertex (const First &first, const Rest&... rest)
        {
            addTransition(first, rest...);
        }

        Vertex &operator << (const Transition &transition)
        {
            m_transitions.push_back(transition);
        }

        Vertex &setAction(const std::function<void()> &action)
        {
            m_action = action;
            return *this;
        }

        bool hasAction()
        {
            return static_cast<bool>(m_action);
        }

        std::list<Transition> m_transitions;
        std::function<void()> m_action;
    private:
        template<typename First, typename ...Rest>
        void addTransition(const First &first, const Rest&... rest)
        {
            m_transitions.push_back(first);
            addTransition(rest...);
        }

        void addTransition(const Transition &first)
        {
            m_transitions.push_back(first);
        }
    };

    AbstractFst(const std::initializer_list<Vertex> &vertices)
    {
        for(const Vertex &vertex : vertices)
        {
            m_vertices.push_back(vertex);
        }
    }

    bool execute(const std::wstring &src)
    {
        if (m_vertices.empty())
            return false;
        m_markedVertices.clear();
        m_visitedVertices = std::stack<Vertex*>();
        m_markedVertices.push_back(0);
        for(const wchar_t ch : src)
        {
            if (!step(ch))
                break;
        }
    }

    bool step(const wchar_t ch)
    {
    }

private:
    std::vector<Vertex> m_vertices;
    std::list<int> m_markedVertices;
    std::stack<int> m_visitedVertices;
};
typedef AbstractFst<> Fst;


} // namespace Transducer
} // namespace PDA

#endif // FST_H
