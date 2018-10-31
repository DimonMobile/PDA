#include <iostream>
#include <cwchar>
#include <cstring>
#include <locale>

#include "Utils/defaults.h"
#include "Utils/settings.h"
#include "Exception/information_exception.h"
#include "Exception/params_exception.h"


int main(int argc, char **argv)
{
    std::setlocale(LC_ALL, "en_US.utf8");

    try
    {
        //PDA::Utils::Settings::Instance().initParams(argc, argv);
        PDA::Transducer::Fst fst = PDA::Utils::Defaults::fst();
        for(;;)
        {
            std::wstring line;
            std::getline(std::wcin, line);
            fst.execute(line.c_str());
        }
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

