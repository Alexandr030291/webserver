
#ifndef WEBSERVER_FUNCTIONREAUEST_H
#define WEBSERVER_FUNCTIONREAUEST_H

class FunctionRequest {
public:
    FunctionRequest(){};
    virtual ~FunctionRequest(){};
    virtual string run(){};
};


#endif //WEBSERVER_FUNCTIONREAUEST_H
