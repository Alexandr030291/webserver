
#ifndef WEBSERVER_HTTP_H
#define WEBSERVER_HTTP_H


#include "filemanager.h"

#define MAX_RESPONSE_HEADER_LEN       4096
#define MIME_IMAGE_JPEG               "image/jpeg"
#define MIME_IMAGE_JPG                "image/jpeg"
#define MIME_IMAGE_PNG                "image/png"
#define MIME_IMAGE_GIF                "image/gif"
#define MIME_SWF                      "application/x-shockwave-flash"
#define MIME_TEXT_HTML                "text/html"
#define MIME_TEXT_CSS                 "text/css"
#define MIME_APPLICATION_JAVASCRIPT     "application/javascript"

#define CONNECTION_KEEP_ALIVE  "keep-alive"
#define CONNECTION_CLOSE  "close"

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

#define DEFAULT_ROOT_PATH  "/home//http-test-suite/"


#define DEFAULT_SERVER_PATH "."
#define NOT_FOUND "/resourse/notFound.html"
#define FORBIDDEN "/resourse/forbidden.html"
#define METHOD_NOT_ALLOWED "/resourse/MethodNotAllowed.html"

static struct DefaultPath {
    char *root_path = (char *) DEFAULT_ROOT_PATH;
    char *server_path = (char *) DEFAULT_SERVER_PATH;
}defaultPath;

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
    URI(char * uriString) {


        bool error = true;
        int len = (int) strlen(uriString);

        char * strEnd =  uriString + len;

        char * pathBegin = NULL;
        char * pathEnd = strEnd;

        char * queryBegin = NULL;
        char * queryEnd = strEnd;

        pathBegin = strchr(uriString, '/');
        if (pathBegin != NULL) {
            queryBegin = strchr(uriString, '?');
            if(queryBegin != NULL) {
                pathEnd = queryBegin;
            }

            dataLen = len + 25;
            data = (char*)malloc((size_t) dataLen);
            path = data;
            int pathStrLen = 0;
            memcpy(path, pathBegin, pathEnd - pathBegin);
            if (*(pathEnd - 1) == '/') {
                lastJ = true;
                char ind[] = "index.html";
                pathStrLen = sizeof(ind) - 1 + (pathEnd - pathBegin);
                memcpy(path + (pathEnd - pathBegin), ind, sizeof(ind));
            }
            else {
                lastJ = false;
                pathStrLen = (int) (pathEnd - pathBegin);
            }
            path[pathStrLen] = '\0';

            if(queryBegin != NULL) {
                query = data + pathStrLen + 1;
                memcpy(query, queryBegin, queryEnd - queryBegin);
                query[queryEnd - queryBegin] = '\0';
            }

            char * buf = (char*)malloc((size_t) (pathStrLen + 25));
            if (utf_8(buf, path) >= 0) {
                memcpy(path, buf, strlen(buf) + 1);
            }
            free(buf);
            error = false;
        }
        else {
            error = true;
        }

    }

    URI(const URI & otherURI) {
        this->dataLen = otherURI.dataLen;
        this->data = (char*)malloc((size_t) this->dataLen);
        memcpy(this->data, otherURI.data, (size_t) this->dataLen);
        this->path = this->data;

        if (otherURI.query != NULL) {
            this->query = this->data + (otherURI.query - otherURI.data);
        }
        else {
            this->query = NULL;
        }
    }

    ~URI() {
        free(data);

    }
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

    void saveStr(char ** saveWhere, char * begin, char * end) {
        *saveWhere = (char*)malloc(end - begin + 20);
        if (*saveWhere) {
            memcpy(*saveWhere, begin, end - begin);
            (*saveWhere)[end - begin] = '\0';
        }

    }

    void init(char * httpRequest) {
        keepAlive = false;
        isValid = true;
        uri = NULL;
        protocol = NULL;
        method = NULL;
        uriString = NULL;

        this->httpRequest = httpRequest;
        char * oldPos = httpRequest;
        char * newPos = httpRequest;
        newPos = strchr(newPos, ' ');
        if(newPos == NULL) {
            isValid = false;
            return;
        }
        saveStr(&method, oldPos, newPos);

        newPos++;
        oldPos = newPos;
        newPos = strchr(newPos, ' ');
        if(newPos == NULL) {
            isValid = false;
            return;
        }
        saveStr(&uriString, oldPos, newPos);

        newPos++;
        oldPos = newPos;
        newPos = strchr(newPos, '\r');
        bool end = false;
        if(newPos == NULL) {
            isValid = false;
            return;
        }

        saveStr(&protocol, oldPos, newPos);
        uri = new URI(uriString);
        keepAlive = strcmp(protocol, PROTOCOL_11) == 0;
    }

    RequestData(char * httpRequest) {
        headersData = (char*)malloc(4089);
        init(httpRequest);

    }

    void clear() {
        if (uri != NULL) free(uri);
        if (method != NULL) free(method);
        if (uriString != NULL) free(uriString);
        if (protocol != NULL) free(protocol);

        keepAlive = false;
        isValid = true;
        uri = NULL;
        protocol = NULL;
        method = NULL;
        uriString = NULL;

    }

    ~RequestData() {
        if (uri != NULL) free(uri);
        if (method != NULL) free(method);
        if (uriString != NULL) free(uriString);
        if (protocol != NULL) free(protocol);

    }
};

struct ResponseData {

    RequestData * request;
    char * header;
    char * data;
    int countHeaderSendedBytes;
    int countDataSendedBytes;
    int headerLen;
    int dataLen;

    void getHTTPResponse() {
        data = NULL;
        headerLen = 0;
        dataLen = 0;
        char * protocol = (char *) PROTOCOL;
        char * status = (char *) STATUS_200;
        FileData * resultFile;
        char * path = request->uri->path;
        bool isGET  = strcmp(request->method, METHOD_GET) == 0;
        bool isHEAD = strcmp(request->method, METHOD_HEAD) == 0;
        bool isPOST = strcmp(request->method, METHOD_POST) == 0;
        if (strstr(request->uri->path, "../")) {
            path = (char *) FORBIDDEN;
            resultFile = getFile(path, defaultPath.server_path, isGET);
            status = (char *) STATUS_403;
        }
        else {
            resultFile = getFile(request->uri->path, defaultPath.root_path, isGET);
        }

        if (!resultFile->success) {
            if(request->uri->lastJ) {
                path = (char *) FORBIDDEN;
                resultFile = getFile(path, defaultPath.server_path, isGET);
                status = (char *) STATUS_403;;
            }
            else {
                path = (char *) NOT_FOUND;
                resultFile = getFile(path, defaultPath.server_path, isGET);
                status = (char *) STATUS_404;
            }
        }
        if (!(isGET || isHEAD)) {
            path = (char *) METHOD_NOT_ALLOWED;
            resultFile = getFile(path, defaultPath.server_path, true);
            status = (char *) STATUS_405;
        }

        char * format;

        char * ptr = strrchr(path, '.');
        if (ptr < 0) {
            format = (char *) MIME_TEXT_HTML;
        }
        else if (strcmp(ptr, ".jpg") == 0) {
            format = (char *) MIME_IMAGE_JPG;
        }
        else if(strcmp(ptr, ".png") == 0) {
            format = (char *) MIME_IMAGE_PNG;
        }
        else if(strcmp(ptr, ".jpeg") == 0) {
            format = (char *) MIME_IMAGE_JPEG;
        }
        else if(strcmp(ptr, ".gif") == 0) {
            format = (char *) MIME_IMAGE_GIF;
        }
        else if(strcmp(ptr, ".html") == 0) {
            format = (char *) MIME_TEXT_HTML;
        }
        else if(strcmp(ptr, ".css") == 0) {
            format = (char *) MIME_TEXT_CSS;
        }
        else if(strcmp(ptr, ".swf") == 0) {
            format = (char *) MIME_SWF;
        }
        else if(strcmp(ptr, ".js") == 0) {
            format = (char *) MIME_APPLICATION_JAVASCRIPT;
        }
        else {
            format = (char *) MIME_TEXT_HTML;
        }


        char * date = http_date(resultFile->date);


        data = NULL;
        dataLen = 0;
        if (!isHEAD) {
            data = resultFile->data;
            dataLen = (int) resultFile->length;
        }
        else {

        }

        headerLen = sprintf(header, "%s %s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n%s%d\r\n\r\n",
                            protocol, status,
                            HEADER_SERVER, SERVER_NAME,
                            HEADER_DATE, date,
                            HEADER_CONNECTION, request->keepAlive?CONNECTION_KEEP_ALIVE:CONNECTION_CLOSE,
                            HEADER_CONTENT_TYPE, format,
                            HEADER_CONTENT_LENGTH, (int) resultFile->length);

    }

    ResponseData(RequestData * request) {
        countHeaderSendedBytes = 0;
        countDataSendedBytes = 0;
        this->request = request;
        header = (char*)malloc(MAX_RESPONSE_HEADER_LEN);
        getHTTPResponse();

    }

    void clear() {
        countHeaderSendedBytes = 0;
        countDataSendedBytes = 0;
    }

    ~ResponseData(){
        if (header != NULL) free(header);
        if (data != NULL) free(data);

    }
};

#endif //WEBSERVER_HTTP_H
