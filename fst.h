#ifndef FST_H
#define FST_H

#include <vector>
#include <list>
#include <set>
#include <iostream>

namespace PDA
{

namespace Transducer
{

template<typename T = wchar_t>
struct AbstractEdge
{
    std::set<T> transitions;

    AbstractEdge() {}
    AbstractEdge(const AbstractEdge<T> &from)
    {
        transitions = from.transitions;
    }
    /// Move semantics constructor
    AbstractEdge(AbstractEdge<T> &&from)
    {
        transitions = std::move(from.transitions);
        std::wcout << "\t***Move c-tor" << std::endl;
    }
    /// Move semantics assignment operator
    AbstractEdge<T>& operator = (AbstractEdge<T> &&from)
    {
        transitions = std::move(from.transitions);
        std::wcout << "\t***Move assign" << std::endl;
    }
    AbstractEdge<T>& operator = (const AbstractEdge<T> &from)
    {
        transitions = from.transitions;
    }

    template<typename ...Rest>
    static AbstractEdge<T> create(T first, Rest ...rest)
    {
        AbstractEdge<T> result = create(rest...);
        result.transitions.insert(first);
        return result;
    }

    static AbstractEdge<T> create(T first)
    {
        AbstractEdge<T> result;
        result.transitions.insert(first);
        return result;
    }
};
typedef AbstractEdge<> Edge;


struct Vertex
{
};


class Fst
{
public:
    Fst();
};


} // namespace Transducer
} // namespace PDA

#endif // FST_H
