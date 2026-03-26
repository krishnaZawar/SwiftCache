#include<windows.h>

#include "syntaxChecks.cpp"

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
    
    return 0;
}