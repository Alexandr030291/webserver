
#ifndef WEBSERVER_REQUESTMAPPING_H
#define WEBSERVER_REQUESTMAPPING_H

#include "main.h"

class UrlKey {
public:
    UrlKey(const PATH, const METHOD);
    string toString();
private:
    PATH path;
    METHOD method;
};

#endif
