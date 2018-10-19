#include <iostream>
#include <cwchar>
#include <cstring>
#include <locale>
#include "fst.h"

int main(int argc, char **argv)
{
    std::setlocale(LC_ALL, "en_US.utf8");
    PDA::Transducer::Vertex vertex;
    vertex << PDA::Transducer::Edge::create(0, L'1', L'2', L'3', L'4', L'5')
           << PDA::Transducer::Edge::create(1, L'a', L'b', L'c', L'd', L'e', L'f');
    // Testing
    for(const PDA::Transducer::Edge &edge : vertex.edges)
    {
        for(const wchar_t transition : edge.transitions)
        {
            std::wcout << "Transition: " << transition << std::endl;
        }
    }
}

