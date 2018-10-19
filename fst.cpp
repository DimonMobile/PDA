#include "fst.h"


namespace PDA
{
namespace Transducer
{

Fst::Fst()
{

}

Edge::Edge()
{

}

Edge::Edge(const Edge &from)
{
    transitionNodeIndex = from.transitionNodeIndex;
    transitions = from.transitions;
}

Edge::Edge(Edge &&from)
{
    transitions = std::move(from.transitions);
    transitionNodeIndex = std::move(from.transitionNodeIndex);
}

Edge &Edge::operator =(Edge &&from)
{
    transitions = std::move(from.transitions);
    transitionNodeIndex = std::move(from.transitionNodeIndex);
    return *this;
}

Vertex::Vertex()
{

}

Vertex::Vertex(Vertex &&from)
{
    edges = std::move(from.edges);
}

Vertex &Vertex::operator =(Vertex &&from)
{
    edges = std::move(from.edges);
    return *this;
}

Vertex &Vertex::operator << (Edge &&from)
{
    edges.push_back(from);
    return *this;
}

Vertex &Vertex::operator <<(const Edge &from)
{
    edges.push_back(from);
    return *this;
}

} // namesoace Transducer
} // namespace PDA
