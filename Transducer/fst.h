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
struct BasicEdge
{
    std::set<T> m_set;

    BasicEdge()
    {
    }

    BasicEdge(const std::initializer_list<T> &initList)
    {
        for(const T &item : initList)
        {
            m_set.insert(item);
        }
    }

    BasicEdge(const BasicEdge<T> &from)
    {
        m_set = from.m_set;
    }
    /// Move semantics constructor
    BasicEdge(BasicEdge<T> &&from)
    {
        m_set = std::move(from.m_set);
    }
    /// Move semantics assignment operator
    BasicEdge<T>& operator = (BasicEdge<T> &&from)
    {
        m_set = std::move(from.m_set);
    }
    BasicEdge<T>& operator = (const BasicEdge<T> &from)
    {
        m_set = from.m_set;
    }

    BasicEdge<T> operator + (const BasicEdge<T> another)
    {
        BasicEdge<T> result;
        result.m_set = m_set;
        result.m_set.insert(another.m_set.begin(), another.m_set.end());
        return result;
    }

    template<typename ...Rest>
    static BasicEdge<T> create(T first, Rest ...rest)
    {
        BasicEdge<T> result = create(rest...);
        result.m_set.insert(first);
        return result;
    }

    static BasicEdge<T> create(T first)
    {
        BasicEdge<T> result;
        result.m_set.insert(first);
        return result;
    }
};
typedef BasicEdge<> Edge;


template<typename T = DEFAULT_FST_UNIT>
class BasicFst
{
public:
    typedef std::pair<int, BasicEdge<T>> Transition;
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

    BasicFst() {}

    BasicFst(const std::initializer_list<Vertex> &vertices)
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
        m_pendingVertices.clear();
        m_pendingVertices.push_back(0);

        for(const wchar_t ch : src)
            if (!step(ch))
                return false;

        for(const int i : m_pendingVertices)
        {
            if (m_vertices[i].hasAction())
            {
                m_vertices[i].m_action();
                return true;
            }
            else if (m_vertices[i].m_transitions.empty())
            {
                return true;
            }
        }
        return false;
    }

    bool step(const wchar_t ch)
    {
        std::list<int> verticesToAdd;
        for(auto pendingVertexIdxIterator = m_pendingVertices.begin(); pendingVertexIdxIterator != m_pendingVertices.end(); ++pendingVertexIdxIterator)
        {
            int currentActiveVertexIdx = *pendingVertexIdxIterator;
            Vertex &currentVertex = m_vertices[currentActiveVertexIdx];
            for(const Transition& currentTransition : currentVertex.m_transitions)
            {
                if(currentTransition.second.m_set.find(ch) != currentTransition.second.m_set.end())
                {
                    verticesToAdd.push_back(currentTransition.first);
                }
            }
        }
        if (verticesToAdd.empty())
            return false;
        m_pendingVertices = std::move(verticesToAdd);
        return true;
    }

private:
    std::vector<Vertex> m_vertices;
    std::list<int> m_pendingVertices;
};
typedef BasicFst<> Fst;


} // namespace Transducer
} // namespace PDA

#endif // FST_H
