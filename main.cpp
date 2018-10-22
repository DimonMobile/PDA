#include <iostream>
#include <cwchar>
#include <cstring>
#include <locale>
#include "fst.h"

int main(int argc, char **argv)
{
    std::setlocale(LC_ALL, "en_US.utf8");

    PDA::Transducer::Edge variadicTemplatesDeclare = PDA::Transducer::Edge::create(L'1', L'2', L'3', L'4', L'5');
    PDA::Transducer::Edge initializerListDeclare = {L'A', L'B', L'C', L'D'};

}

