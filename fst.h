#ifndef FST_H
#define FST_H

#include <vector>
#include <list>
#include <set>

namespace PDA
{

namespace Transducer
{

struct Edge
{
    std::set<wchar_t> transitions;
    int transitionNodeIndex;

    Edge();
    Edge(const Edge &from);
    /// Move semantics constructor
    Edge(Edge &&from);
    /// Move semantics assignment operator
    Edge& operator = (Edge &&from);

    template<typename First, typename... Rest>
    static Edge create(int transitionNodeIndex, const First& first, const Rest&... rest)
    {
        Edge result;
        result.transitionNodeIndex = transitionNodeIndex;
        result.transitions = buildEdge(first, rest...);
        return result;
    }
private:
    template<typename T>
    static std::set<wchar_t> buildEdge(const T& t)
    {
        std::set<wchar_t> result;
        result.insert(t);
        return result;
    }

    template<typename First, typename... Rest>
    static std::set<wchar_t> buildEdge(const First& first, const Rest&... rest)
    {
        std::set<wchar_t> result = buildEdge(rest...);
        result.insert(first);
        return result;
    }
};


struct Vertex
{
    std::list<Edge> edges;

    Vertex();
    /// Move semantics constructor
    Vertex(Vertex &&from);
    /// Move semantics assignment operator
    Vertex &operator = (Vertex &&from);
    Vertex &operator << (Edge &&from);
    Vertex &operator << (const Edge &from);
};


class Fst
{
public:
    Fst();
};


} // namespace Transducer
} // namespace PDA

#endif // FST_H
