#include "UrlKey.h"

string trans_to_string(METHOD type){
    switch (type){
        case GET:
            return "GET";
        case HEARD:
            return "HEARD";
        default:
            return "";
    }
}

UrlKey::UrlKey(const string _path,
               const METHOD _method){
    path=_path;
    method=_method;
}

string UrlKey::toString() {
    return path+"."+trans_to_string(method);
}