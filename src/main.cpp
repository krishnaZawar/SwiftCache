#include<iostream>
#include "parser.cpp"
#include "database/database.cpp"

using std::exception;
using std::cout;
using std::getline;
using std::cin;
using std::endl;

using std::string;

int main(){
    string command = "";

    Database db = Database();
    Parser parser = Parser(&db);
    
    while(true){
        cout<<">> ";
        getline(cin, command);
        if(command == "EXIT"){
            break;
        }
        try{  
            cout<<parser.parseCommand(command)<<endl;
            db.printLoadFactor();
        }
        catch(string &err){
            cout<<err<<endl;
        }
        catch(exception &e){
            cout<<e.what()<<endl;
        }
    }

    return 0;
}