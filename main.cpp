#include <iostream>
#include <cwchar>
#include <cstring>
#include <locale>
#include <codecvt>

#include "Utils/defaults.h"
#include "Utils/settings.h"
#include "Exception/information_exception.h"
#include "Exception/params_exception.h"
#include "Transducer/preprocessor.h"
#include "Transducer/tokenizer.h"


int main(int argc, char **argv)
{
    std::setlocale(LC_ALL, "en_US.UTF-8");
    std::locale loc(std::locale(), new std::codecvt_utf8<wchar_t>);
    std::locale::global(loc);

    try
    {
        PDA::Utils::Settings::Instance().initParams(argc, argv);
        PDA::Transducer::Preprocessor preprocessor(PDA::Utils::Settings::Instance().sourceFilePath());
        PDA::Transducer::Tokenizer tokenizer(preprocessor.source());
        PDA::Transducer::Mfst mfst(PDA::Utils::Defaults::mfst());
        mfst.setSource(tokenizer.tokens());
        mfst.step();
        mfst.step();
        mfst.step();

        //tokenizer.printTokens();
        //tokenizer.printIdentifiers();
    }
    catch (const PDA::Exception::InformationException &e)
    {
        std::wcout << e.what() << std::endl;
    }
    catch (const PDA::Exception::Exception &e)
    {
        std::wcerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

