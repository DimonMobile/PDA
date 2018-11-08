#include "defaults.h"
#include <iostream>

#define _ std::make_pair

namespace PDA
{
namespace Utils
{
namespace Defaults
{
    namespace Constants
    {
        const wchar_t varToken          = L'v';
        const wchar_t doubleToken       = L'd';
        const wchar_t intToken          = L'i';
        const wchar_t endToken          = L'}';
        const wchar_t boolToken         = L'b';
        const wchar_t printToken        = L'p';
        const wchar_t returnToken       = L'r';
        const wchar_t whileToken        = L'w';
        const wchar_t mainToken         = L'm';
        const wchar_t breakToken        = L'|';
        const wchar_t voidToken         = L' ';
        const wchar_t stringToken       = L's';
        const wchar_t askToken          = L'a';
        const wchar_t createToken       = L'c';
    }

    Transducer::Fst fst()
    {
        Transducer::Fst result;
        using namespace PDA::Transducer;
        Edge digitEdge = Edge::create(L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7', L'8', L'9');
        Edge alphaEdge = Edge::create(L'а',L'б',L'в',L'г',L'д',L'е',L'ё',L'ж',L'з',L'и',L'й',L'к',L'л',L'м',L'н',L'о',L'п',L'р',L'с',L'т',L'у',L'ф',L'х',L'ц',L'ч',L'ш',L'щ',L'ь',L'ы',L'ъ',L'э',L'ю',L'я',L'А',L'Б',L'В',L'Г',L'Д',L'Е',L'Ё',L'Ж',L'З',L'И',L'Й',L'К',L'Л',L'М',L'Н',L'О',L'П',L'Р',L'С',L'Т',L'У',L'Ф',L'Х',L'Ц',L'Ч',L'Ш',L'Щ',L'Ь',L'Ы',L'Ъ',L'Э',L'Ю',L'Я',L'a',L'b',L'c',L'd',L'e',L'f',L'g',L'h',L'i',L'j',L'k',L'l',L'm',L'n',L'o',L'p',L'q',L'r',L's',L't',L'u',L'v',L'w',L'x',L'y',L'z',L'A',L'B',L'C',L'D',L'E',L'F',L'G',L'H',L'I',L'J',L'K',L'L',L'M',L'N',L'O',L'P',L'Q',L'R',L'S',L'T',L'U',L'V',L'W',L'X',L'Y',L'Z');
        Edge spaceEdge = Edge::create(L' ', L'_');
        Edge miscEdge = Edge::create(L'~', L'`', L'!', L'#', L'$', L'%', L'^', L'&', L'*', L'(', L')', L'-', L'+', L'=', L'/', L'\\', L'|', L'}', L'{', L'[', L']', L'.', L',', L'?', L'№', ';');
        Edge printableEdge = digitEdge + alphaEdge + spaceEdge + miscEdge;
        Edge hexDigitEdge = digitEdge + Edge::create(L'A', L'B', L'C', L'D', L'E', L'F', L'a', L'b', L'c', L'd', L'e', L'f');
        //абвгдеёжхийглмнопрстуфхцчшщьыъэюяАБВГДЕЁЖХИЙКЛМНОПРСТУФХЦЧШЩЬЫЪЭЮЯabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ
        Edge alphaDigitSpaceEdge = digitEdge + alphaEdge + Edge::create(L'_');
        Edge alphaSpaceEdge = alphaEdge + Edge::create(L'_');

        result = {
/* 0 */             Fst::Vertex( _(1, Edge::create(L'@')), _(15, Edge::create(L'б')), _(20, Edge::create(L'в')), _(27, Edge::create(L'д')), _(34, Edge::create(L'к')), _(39, Edge::create(L'л')), _(47, Edge::create(L'п')), _(71, Edge::create(L'р')), _(76, Edge::create(L'с')), _(82, Edge::create(L'ц')), _(95, digitEdge), _(98, Edge::create(L'0')), _(101, alphaSpaceEdge), _(102, Edge::create(L'+')), _(103, Edge::create(L'-')), _(104, Edge::create(L'*')), _(105, Edge::create(L'/')), _(106, Edge::create(L'\"') ), _(108, Edge::create(L';')), _(109, Edge::create(L':')), _(110, Edge::create(L'=')), _(112, Edge::create(L'(')), _(113, Edge::create(L')')), _(114, Edge::create(L'<')), _(116, Edge::create('>')) )
/* 1 */         ,   Fst::Vertex( _(2, Edge::create(L'н')), _(7, Edge::create(L'у')) )
/* 2 */         ,   Fst::Vertex( _(3, Edge::create(L'у')) )
/* 3 */         ,   Fst::Vertex( _(4, Edge::create(L'м')) )
/* 4 */         ,   Fst::Vertex( _(5, Edge::create(L'а')) )
/* 5 */         ,   Fst::Vertex( _(6, Edge::create(L'р')) )
/* 6 */         ,   Fst::Vertex().setAction([&](){std::wcerr << L"@нумар detected" << std::endl;})
/* 7 */         ,   Fst::Vertex( _(8, Edge::create(L'к')) )
/* 8 */         ,   Fst::Vertex( _(9, Edge::create(L'л')) )
/* 9 */         ,   Fst::Vertex( _(10, Edge::create(L'ю')) )
/*10 */         ,   Fst::Vertex( _(11, Edge::create(L'ч')) )
/*11 */         ,   Fst::Vertex( _(12, Edge::create(L'ы')) )
/*12 */         ,   Fst::Vertex( _(13, Edge::create(L'ц')) )
/*13 */         ,   Fst::Vertex( _(14, Edge::create(L'ь')) )
/*14 */         ,   Fst::Vertex().setAction([&](){std::wcerr << L"@уключыць detected" << std::endl;})
/*15 */         ,   Fst::Vertex( _(16, Edge::create(L'у')) )
/*16 */         ,   Fst::Vertex( _(17, Edge::create(L'д')) )
/*17 */         ,   Fst::Vertex( _(18, Edge::create(L'з')) )
/*18 */         ,   Fst::Vertex( _(19, Edge::create(L'е')) )
/*19 */         ,   Fst::Vertex().setAction([&](){ result.userData.asWChar = Constants::varToken; })
/*20 */         ,   Fst::Vertex( _(21, Edge::create(L'я')) )
/*21 */         ,   Fst::Vertex( _(22, Edge::create(L'р')) )
/*22 */         ,   Fst::Vertex( _(23, Edge::create(L'н')) )
/*23 */         ,   Fst::Vertex( _(24, Edge::create(L'у')) )
/*24 */         ,   Fst::Vertex( _(25, Edge::create(L'ц')) )
/*25 */         ,   Fst::Vertex( _(26, Edge::create(L'ь')) )
/*26 */         ,   Fst::Vertex().setAction([&](){result.userData.asWChar = Constants::returnToken;})
/*27 */         ,   Fst::Vertex( _(28, Edge::create(L'р')) )
/*28 */         ,   Fst::Vertex( _(29, Edge::create(L'о')) )
/*29 */         ,   Fst::Vertex( _(30, Edge::create(L'б')) )
/*30 */         ,   Fst::Vertex( _(31, Edge::create(L'н')) )
/*31 */         ,   Fst::Vertex( _(32, Edge::create(L'а')) )
/*32 */         ,   Fst::Vertex( _(33, Edge::create(L'е')) )
/*33 */         ,   Fst::Vertex().setAction([&](){result.userData.asWChar = Constants::doubleToken;})
/*34 */         ,   Fst::Vertex( _(35, Edge::create(L'а')) )
/*35 */         ,   Fst::Vertex( _(36, Edge::create(L'н')) )
/*36 */         ,   Fst::Vertex( _(37, Edge::create(L'е')) )
/*37 */         ,   Fst::Vertex( _(38, Edge::create(L'ц')) )
/*38 */         ,   Fst::Vertex().setAction([&](){result.userData.asWChar = Constants::endToken;})
/*39 */         ,   Fst::Vertex( _(40, Edge::create(L'а')) )
/*40 */         ,   Fst::Vertex( _(41, Edge::create(L'г')) )
/*41 */         ,   Fst::Vertex( _(42, Edge::create(L'i')) )
/*42 */         ,   Fst::Vertex( _(43, Edge::create(L'ч')) )
/*43 */         ,   Fst::Vertex( _(44, Edge::create(L'н')) )
/*44 */         ,   Fst::Vertex( _(45, Edge::create(L'а')) )
/*45 */         ,   Fst::Vertex( _(46, Edge::create(L'е')) )
/*46 */         ,   Fst::Vertex().setAction([&](){result.userData.asWChar = Constants::boolToken;})
/*47 */         ,   Fst::Vertex( _(48, Edge::create(L'а')), _(67, Edge::create(L'у')) )
/*48 */         ,   Fst::Vertex( _(49, Edge::create(L'к')), _(56, Edge::create(L'л')), _(62, Edge::create(L'ч')) )
/*49 */         ,   Fst::Vertex( _(50, Edge::create(L'а')), _(53, Edge::create(L'у')) )
/*50 */         ,   Fst::Vertex( _(51, Edge::create(L'ж')) )
/*51 */         ,   Fst::Vertex( _(52, Edge::create(L'ы')) )
/*52 */         ,   Fst::Vertex().setAction([&](){result.userData.asWChar = Constants::printToken;})
/*53 */         ,   Fst::Vertex( _(54, Edge::create(L'л')) )
/*54 */         ,   Fst::Vertex( _(55, Edge::create(L'ь')) )
/*55 */         ,   Fst::Vertex().setAction([&](){result.userData.asWChar = Constants::whileToken;})
/*56 */         ,   Fst::Vertex( _(57, Edge::create(L'а')) )
/*57 */         ,   Fst::Vertex( _(58, Edge::create(L'м')) )
/*58 */         ,   Fst::Vertex( _(59, Edge::create(L'а')) )
/*59 */         ,   Fst::Vertex( _(60, Edge::create(L'ц')) )
/*60 */         ,   Fst::Vertex( _(61, Edge::create(L'ь')) )
/*61 */         ,   Fst::Vertex().setAction([&](){result.userData.asWChar = Constants::breakToken;})
/*62 */         ,   Fst::Vertex( _(63, Edge::create(L'а')) )
/*63 */         ,   Fst::Vertex( _(64, Edge::create(L'т')) )
/*64 */         ,   Fst::Vertex( _(65, Edge::create(L'а')) )
/*65 */         ,   Fst::Vertex( _(66, Edge::create(L'к')) )
/*66 */         ,   Fst::Vertex().setAction([&](){result.userData.asWChar = Constants::mainToken;})
/*67 */         ,   Fst::Vertex( _(68, Edge::create(L'с')) )
/*68 */         ,   Fst::Vertex( _(69, Edge::create(L'т')) )
/*69 */         ,   Fst::Vertex( _(70, Edge::create(L'а')) )
/*70 */         ,   Fst::Vertex().setAction([&](){result.userData.asWChar = Constants::voidToken;})
/*71 */         ,   Fst::Vertex( _(72, Edge::create(L'а')) )
/*72 */         ,   Fst::Vertex( _(73, Edge::create(L'д')) )
/*73 */         ,   Fst::Vertex( _(74, Edge::create(L'о')) )
/*74 */         ,   Fst::Vertex( _(75, Edge::create(L'к')) )
/*75 */         ,   Fst::Vertex().setAction([&](){result.userData.asWChar = Constants::stringToken;})
/*76 */         ,   Fst::Vertex( _(77, Edge::create(L'п')), _(83, Edge::create(L'т')) )
/*77 */         ,   Fst::Vertex( _(78, Edge::create(L'ы')) )
/*78 */         ,   Fst::Vertex( _(79, Edge::create(L'т')) )
/*79 */         ,   Fst::Vertex( _(80, Edge::create(L'а')) )
/*80 */         ,   Fst::Vertex( _(81, Edge::create(L'й')) )
/*81 */         ,   Fst::Vertex().setAction([&](){result.userData.asWChar = Constants::askToken;})
/*82 */         ,   Fst::Vertex( _(90, Edge::create(L'i')), _(91, Edge::create(L'э')) )
/*83 */         ,   Fst::Vertex( _(84, Edge::create(L'в')) )
/*84 */         ,   Fst::Vertex( _(85, Edge::create(L'а')) )
/*85 */         ,   Fst::Vertex( _(86, Edge::create(L'р')) )
/*86 */         ,   Fst::Vertex( _(87, Edge::create(L'ы')) )
/*87 */         ,   Fst::Vertex( _(88, Edge::create(L'ц')) )
/*88 */         ,   Fst::Vertex( _(89, Edge::create(L'ь')) )
/*89 */         ,   Fst::Vertex().setAction([&](){result.userData.asWChar = Constants::createToken;})
/*90 */         ,   Fst::Vertex().setAction([&](){std::wcout << L"цi detected" << std::endl;})
/*91 */         ,   Fst::Vertex( _(92, Edge::create(L'л')) )
/*92 */         ,   Fst::Vertex( _(93, Edge::create(L'а')) )
/*93 */         ,   Fst::Vertex( _(94, Edge::create(L'е')) )
/*94 */         ,   Fst::Vertex().setAction([&](){result.userData.asWChar = Constants::intToken;})
/*95 */         ,   Fst::Vertex( _(95, digitEdge ), _(96, Edge::create(L'.')) ).setAction([&](){std::wcout << L"целый литерал detected" << std::endl;})
/*96 */         ,   Fst::Vertex( _(97, digitEdge) )
/*97 */         ,   Fst::Vertex( _(97, digitEdge) ).setAction([&](){std::wcout << L"вещественный литерал detected" << std::endl;})
/*98 */         ,   Fst::Vertex( _(99, Edge::create(L'x', L'X')) )
/*99 */         ,   Fst::Vertex( _(100, hexDigitEdge) )
/*100*/         ,   Fst::Vertex( _(100, hexDigitEdge) ).setAction([&](){std::wcout << L"цэлый шестнадцатеричный литерал detected" << std::endl;})
/*101*/         ,   Fst::Vertex( _(101, alphaDigitSpaceEdge) ).setAction([&](){std::wcout << L"идентификатор detected" << std::endl;})
/*102*/         ,   Fst::Vertex().setAction([&](){std::wcout << L"+ detected" << std::endl;})
/*103*/         ,   Fst::Vertex().setAction([&](){std::wcout << L"- detected" << std::endl;})
/*104*/         ,   Fst::Vertex().setAction([&](){std::wcout << L"* detected" << std::endl;})
/*105*/         ,   Fst::Vertex().setAction([&](){std::wcout << L"/ detected" << std::endl;})
/*106*/         ,   Fst::Vertex( _(106, printableEdge), _(107, Edge::create(L'\"')) )
/*107*/         ,   Fst::Vertex().setAction([&](){std::wcout << L"строковый литерал detected" << std::endl;})
/*108*/         ,   Fst::Vertex().setAction([&](){std::wcout << L"; detected" << std::endl;})
/*109*/         ,   Fst::Vertex().setAction([&](){std::wcout << L": detected" << std::endl;})
/*110*/         ,   Fst::Vertex( _(111, Edge::create(L'=')) ).setAction([&](){std::wcout << L"= detected" << std::endl;})
/*111*/         ,   Fst::Vertex().setAction([&](){std::wcout << L"== detected" << std::endl;})
/*112*/         ,   Fst::Vertex().setAction([&](){std::wcout << L"( detected" << std::endl;})
/*113*/         ,   Fst::Vertex().setAction([&](){std::wcout << L") detected" << std::endl;})
/*114*/         ,   Fst::Vertex( _(115, Edge::create(L'=')) ).setAction([&](){std::wcout << L"< detected" << std::endl;})
/*115*/         ,   Fst::Vertex().setAction([&](){std::wcout << L"<= detected" << std::endl;})
/*116*/         ,   Fst::Vertex( _(117, Edge::create(L'=')) ).setAction([&](){std::wcout << L"> detected" << std::endl;})
/*117*/         ,   Fst::Vertex().setAction([&](){std::wcout << L">= detected" << std::endl;})
        };
        return result;
    }
} // namesapce Defaults
} // naemsapce Utils
} // namespace PDA
