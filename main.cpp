#include <iostream>
#include <cwchar>
#include <cstring>
#include <locale>

#include "Transducer/fst.h"
#include "Utils/settings.h"
#include "Exception/information_exception.h"


int main(int argc, char **argv)
{
    std::setlocale(LC_ALL, "en_US.utf8");

    try
    {
        PDA::Utils::Settings::Instance().initParams(argc, argv);

        using namespace PDA::Transducer;
        Edge variadicTemplatesDeclare = Edge::create(L'1', L'2', L'3', L'4', L'5');
        Edge initializerListDeclare = {L'A', L'B', L'C', L'D'};
        Fst fst = {
                Fst::Vertex(    // 0
                    std::make_pair(0, Edge::create(L'a', L'b'))
                ,   std::make_pair(1, Edge::create(L'a', L'b')) )
            ,   Fst::Vertex().setAction([](){std::wcout << L"Hello World!" << std::endl;})
        };
    } catch (const PDA::Exception::InformationException &e) {
        std::wcout << e.what() << std::endl;
    }


}

