#ifdef _WIN32
#include<windows.h>
#else
#include<clocale>
#endif

#include "syntaxChecks.cpp"
#include "sanityChecks.cpp"
#include "WALChecks.cpp"

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#else
    std::setlocale(LC_ALL, "en_US.UTF-8");
#endif

    cout << "Initializing tester..." << endl;
    cout << "Loading tests..." << endl << endl;

    syntaxCheck_stringCommands();
    cout << endl;
    syntaxCheck_genericCommands();
    cout << endl;
    syntaxCheck_listCommands();
    cout << endl;
    syntaxCheck_hashCommands();
    cout << endl;

    sanityCheck_stringCommands();
    cout << endl;
    sanityCheck_genericCommands();
    cout << endl;
    sanityCheck_listCommands();
    cout << endl;
    sanityCheck_hashCommands();
    cout << endl;
    
    sanityCheck_genericFlows();
    cout << endl;

    sanityCheck_walPersistence();
    cout << endl;

    return 0;
}