#include<string>
#include<deque>
#include<vector>
#include "../../base/errors.h"

#ifndef ListDatatype_class
#define ListDatatype_class

using std::string;
using std::deque;
using std::vector;

class ListDatatype{
    private:
        deque<string> list;

    public:
        ListDatatype(){
            list = deque<string> ();
        }
        
        inline void assertIdxBounds(int idx){
            if(idx < 0 || idx >= list.size()){
                throw ERR_INDEX_OUT_OF_BOUNDS;
            }
        }

        inline void pushLeft(string value){
            list.push_front(value);
        }
        inline void pushRight(string value){
            list.push_back(value);
        }
        void pushAtIdx(int idx, string value){
            assertIdxBounds(idx);
            if(idx == 0){
                pushLeft(value);
            }
            else if(idx == list.size()-1){
                pushRight(value);
            }
            else{
                list.insert(list.begin()+idx, value);
            }
        }

        inline string popLeft(){
            string val = list.front();
            list.pop_front();
            return val;
        }
        inline string popRight(){
            string val = list.back();
            list.pop_back();
            return val;
        }
        string popAtIdx(int idx){
            assertIdxBounds(idx);
            if(idx == 0){
                return popLeft();
            }
            if(idx == list.size()){
                return popRight();
            }
            string val = list[idx];
            list.erase(list.begin()+idx);
            return val;
        }

        inline int len(){
            return list.size();
        }

        inline string getIdx(int idx){
            assertIdxBounds(idx);
            return list[idx];
        }

        vector<string> buildCommands(string key) {
            vector<string> commands;
            commands.push_back("DEL " + key);
            commands.push_back("RPUSH " + key);
            for(auto &item: list){
                commands.back() += " " + item;
            }
            return commands;
        }
};

#endif // ListDatatype_class