#include<windows.h>

#include "syntaxChecks.cpp"
#include "sanityChecks.cpp"

int main() {
    SetConsoleOutputCP(CP_UTF8);

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

    return 0;
}