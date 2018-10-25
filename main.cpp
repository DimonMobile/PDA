#include <iostream>
#include <cwchar>
#include <cstring>
#include <locale>

#include "Transducer/fst.h"
#include "Utils/settings.h"
#include "Exception/information_exception.h"
#include "Exception/params_exception.h"


int main(int argc, char **argv)
{
    std::setlocale(LC_ALL, "en_US.utf8");

    try
    {
        PDA::Utils::Settings::Instance().initParams(argc, argv);

        using namespace PDA::Transducer;
        Edge digitEdge = Edge::create(L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9');
        //абвгдеёжхийглмнопрстуфхцчшщьыъэюяАБВГДЕЁЖХИЙКЛМНОПРСТУФХЦЧШЩЬЫЪЭЮЯabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ
        Edge alphaEdge = Edge::create(L'а',L'б',L'в',L'г',L'д',L'е',L'ё',L'ж',L'х',L'и',L'й',L'г',L'л',L'м',L'н',L'о',L'п',L'р',L'с',L'т',L'у',L'ф',L'х',L'ц',L'ч',L'ш',L'щ',L'ь',L'ы',L'ъ',L'э',L'ю',L'я',L'А',L'Б',L'В',L'Г',L'Д',L'Е',L'Ё',L'Ж',L'Х',L'И',L'Й',L'К',L'Л',L'М',L'Н',L'О',L'П',L'Р',L'С',L'Т',L'У',L'Ф',L'Х',L'Ц',L'Ч',L'Ш',L'Щ',L'Ь',L'Ы',L'Ъ',L'Э',L'Ю',L'Я',L'a',L'b',L'c',L'd',L'e',L'f',L'g',L'h',L'i',L'j',L'k',L'l',L'm',L'n',L'o',L'p',L'q',L'r',L's',L't',L'u',L'v',L'w',L'x',L'y',L'z',L'A',L'B',L'C',L'D',L'E',L'F',L'G',L'H',L'I',L'J',L'K',L'L',L'M',L'N',L'O',L'P',L'Q',L'R',L'S',L'T',L'U',L'V',L'W',L'X',L'Y',L'Z');
        Fst fst = {
                Fst::Vertex( std::make_pair(1, Edge::create(L's')) ).setAction( []() {std::wcout << L"First" << std::endl;} )                                    // 0
            ,   Fst::Vertex( std::make_pair(2, Edge::create(L'q')) )                                    // 1
            ,   Fst::Vertex( std::make_pair(3, Edge::create(L'r')) )                // 2
            ,   Fst::Vertex( std::make_pair(4, Edge::create(L't')) ).setAction( [](){std::wcout << L"Курлык" << std::endl;} )                                    // 3
            ,   Fst::Vertex( std::make_pair(5, Edge::create(L'(')) )                                    // 4
            ,   Fst::Vertex( std::make_pair(6, alphaEdge), std::make_pair(5, digitEdge) )               // 5
            ,   Fst::Vertex( std::make_pair(7, Edge::create(L')') ) )                                   // 6
            ,   Fst::Vertex().setAction( [](){std::wcout << L"Full" << std::endl; } )                                                                           // 7
        };
        std::wcout << "***FinalStepTranducer: " << fst.execute(L"sqrt(ab)") << std::endl;
    }
    catch (const PDA::Exception::InformationException &e)
    {
        std::wcout << e.what() << std::endl;
    }
    catch (const PDA::Exception::ParamsException &e)
    {
        std::wcerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

