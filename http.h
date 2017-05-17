#ifndef HTML_H
#define HTML_H

#include "filemanager.h"

#define MAX_RESPONSE_HEADER_LEN 4096

#define STATUS_200  "200 OK"
#define STATUS_403  "403 Forbidden"
#define STATUS_404  "404 Not Found"
#define STATUS_405  "405 Method Not Allowed"

#define PROTOCOL  "HTTP/1.1"
#define PROTOCOL_10  "HTTP/1.0"
#define PROTOCOL_11  "HTTP/1.1"
#define SERVER_NAME  "SoskovAO"

#define HEADER_STATUS_CODE  "Status Code: "
#define HEADER_CONTENT_TYPE  "Content-Type: "
#define HEADER_CONTENT_LENGTH  "Content-Length: "
#define HEADER_SERVER  "Server: "
#define HEADER_DATE  "Date: "
#define HEADER_CONNECTION  "Connection: "

#define METHOD_GET  "GET"
#define METHOD_HEAD  "HEAD"
#define METHOD_POST  "POST"

#define NOT_FOUND "/recourse/notFound.html"
#define FORBIDDEN "/recourse/forbidden.html"
#define METHOD_NOT_ALLOWED "/recourse/MethodNotAllowed.html"

#define default_root_path "/home/alexandr/CLionProjects/http-test-suite/"
#define default_server_path "."

static char * root_path = default_root_path;
static char * server_path = default_server_path;

short ctoi(char ch);

short oxToChar(char * oxCharCode);

short utf_8(char * dst, char * src);

char * http_date(tm * time);

struct URI {
    char * path;
    char * query;
    char * data;
    int dataLen;
    bool lastJ;
    URI(char * uriString);
    URI(const URI & otherURI);
    ~URI();
};

struct RequestData {
    char * httpRequest;
    char * uriString;
    char * protocol;
    URI * uri;
    char * method;
    char ** headers;
    char * headersData;
    bool isValid;
    bool keepAlive;

    void saveStr(char ** saveWhere, char * begin, char * end) ;

    void init(char * httpRequest);

    RequestData(char * httpRequest);

    void clear();

    ~RequestData();
};

struct ResponseData {

    RequestData * request;
    char * header;
    char * data;
    int countHeaderSendedBytes;
    int countDataSendedBytes;
    int headerLen;
    int dataLen;

    void getHTTPResponse();

    ResponseData(RequestData * request);

    void clear();

    ~ResponseData();


};
#endif // HTML_H
