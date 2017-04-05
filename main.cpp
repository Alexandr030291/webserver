#include <fstream>
#include "ServerEpoll.h"
//using namespace std;

class ReadFile:public FunctionRequest{
private:
    ifstream *file;
public:
    ReadFile(string name){
        this->file = new ifstream(name);
    };
    ~ReadFile(){
        file->close();
    };
    string run(){
        string result;
        (*file) >> result;
        return result;
    };
};

int main(){
    try{
        ServerEpoll server_epoll;
        // server_epoll.add_url(UrlKey("/",GET).toString(),new ReadFile("index.html"));
        server_epoll.run();
    }
    catch (const char* message){
        cout<<message<<endl;
    }
    return 0;
}

