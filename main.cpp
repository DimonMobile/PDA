#include <iostream>
#include <cwchar>
#include <cstring>
#include <locale>
#include "fst.h"

int main(int argc, char **argv)
{
    std::setlocale(LC_ALL, "en_US.utf8");
    PDA::Transducer::Edge edge = PDA::Transducer::Edge::create(L'1', L'2', L'3', L'a', L'5');
    std::wcout << edge.transitions.size() << std::endl;
    for(auto &item : edge.transitions)
        std::wcout << item << std::endl;
}

