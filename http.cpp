#include "http.h"


short ctoi(char ch) {
    if(ch >= '0' && ch <= '9') {
        return ch - '0';
    }
    else if(ch >= 'A' && ch <= 'F') {
        return (short) (ch - 'A' + 10);
    }
    else if(ch >= 'a' && ch <= 'f') {
        return (short) (ch - 'a' + 10);
    }
    else {
        return -1;
    }

}

short oxToChar(char * oxCharCode) {
    short result;
    short chh = ctoi(*(oxCharCode));
    short chl = ctoi(*(oxCharCode + 1));
    if (chh >= 0 && chl >= 0) {
        result = (chh << 4)+ chl;
    }
    else {
        result = -1;
    }
    return result;

}

short utf_8(char * dst, char * src) {
    char * srcPointer = strchr(src, '%');
    while(srcPointer != NULL) {
        memcpy(dst, src, srcPointer - src);
        dst += srcPointer - src;
        src = srcPointer + 3;

        if(((*(srcPointer + 1)) != '\0') && ((*(srcPointer + 2)) != '\0')) {
            short res = oxToChar(srcPointer + 1);
            if (res < 0) {
                return -1;
            }
            *dst = (char)res;
            dst += 1;
            srcPointer += 3;
        }
        else {
            return -2;
        }
        srcPointer = strchr(srcPointer, '%');
    }

    memcpy(dst, src, strlen(src) + 1);
    return 0;

}

char * http_date(tm * time) {
    char * result = (char*)malloc(240);
    const char day_name[][4] =  {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    const char month_name[][4] = {"Jan", "Feb", "Mar", "Apr",
                                  "May", "Jun", "Jul", "Aug",
                                  "Sep", "Oct", "Nov", "Dec"};
    sprintf(result, "%s, %2d %s %4d %2d:%2d:%2d GMT",
            day_name[time->tm_wday], time->tm_mday,
            month_name[time->tm_mon], 1900 + time->tm_year,
            time->tm_hour, time->tm_min, time->tm_sec);
    return result;
}

URI::URI(char * uriString) {


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
    }

}

URI::URI(const URI & otherURI) {
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

URI::~URI() {
    free(data);
}

void RequestData::saveStr(char ** saveWhere, char * begin, char * end) {
    *saveWhere = (char*)malloc(end - begin + 20);
    if (*saveWhere) {
        memcpy(*saveWhere, begin, end - begin);
        (*saveWhere)[end - begin] = '\0';
    }

}

void RequestData::init(char * httpRequest) {
    keepAlive = false;
    isValid = true;
    uri = NULL;
    protocol = NULL;
    method = NULL;
    uriString = NULL;

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
    if(newPos == NULL) {
        isValid = false;
        return;
    }

    saveStr(&protocol, oldPos, newPos);
    uri = new URI(uriString);
    keepAlive = strcmp(protocol, PROTOCOL_11) == 0;
}

RequestData::RequestData(char * httpRequest) {
    (char*)malloc(4089);
    init(httpRequest);

}

void RequestData::clear() {
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

RequestData::~RequestData() {
    if (uri != NULL) free(uri);
    if (method != NULL) free(method);
    if (uriString != NULL) free(uriString);
    if (protocol != NULL) free(protocol);

}

void ResponseData::getHTTPResponse() {
    char MIME_IMAGE_JPEG[]              = "image/jpeg";
    char MIME_IMAGE_JPG[]               = "image/jpeg";
    char MIME_IMAGE_PNG[]               = "image/png";
    char MIME_IMAGE_GIF[]               = "image/gif";
    char MIME_SWF[]                     = "application/x-shockwave-flash";
    char MIME_TEXT_HTML[]               = "text/html";
    char MIME_TEXT_CSS[]                = "text/css";
    char MIME_APPLICATION_JAVASCRIPT[]  = "application/javascript";

    char CONNECTION_KEEP_ALIVE[] = "keep-alive";
    char CONNECTION_CLOSE[] = "close";

    data = NULL;
    headerLen = 0;
    dataLen = 0;
    char * protocol = (char *) PROTOCOL;
    char * status = (char *) STATUS_200;
    FileData * resultFile;
    char * path = request->uri->path;
    bool isGET  = strcmp(request->method, METHOD_GET) == 0;
    bool isHEAD = strcmp(request->method, METHOD_HEAD) == 0;
    //bool isPOST = strcmp(request->method, METHOD_POST) == 0;
    if (strstr(request->uri->path, "../")) {
        path = (char *) FORBIDDEN;
        resultFile = getFile(path, server_path, isGET);
        status = (char *) STATUS_403;
    }
    else {
        resultFile = getFile(request->uri->path, root_path, isGET);
    }

    if (!resultFile->success) {
        if(request->uri->lastJ) {
            path = (char *) FORBIDDEN;
            resultFile = getFile(path, server_path, isGET);
            status = (char *) STATUS_403;;
        }
        else {
            path = (char *) NOT_FOUND;
            resultFile = getFile(path, server_path, isGET);
            status = (char *) STATUS_404;
        }
    }
    if (!(isGET || isHEAD)) {
        path = (char *) METHOD_NOT_ALLOWED;
        resultFile = getFile(path, server_path, true);
        status = (char *) STATUS_405;
    }

    char * format;

    char * ptr = strrchr(path, '.');
    if (ptr < 0) {
        format = MIME_TEXT_HTML;
    }
    else if (strcmp(ptr, ".jpg") == 0) {
        format = MIME_IMAGE_JPG;
    }
    else if(strcmp(ptr, ".png") == 0) {
        format = MIME_IMAGE_PNG;
    }
    else if(strcmp(ptr, ".jpeg") == 0) {
        format = MIME_IMAGE_JPEG;
    }
    else if(strcmp(ptr, ".gif") == 0) {
        format = MIME_IMAGE_GIF;
    }
    else if(strcmp(ptr, ".html") == 0) {
        format = MIME_TEXT_HTML;
    }
    else if(strcmp(ptr, ".css") == 0) {
        format = MIME_TEXT_CSS;
    }
    else if(strcmp(ptr, ".swf") == 0) {
        format = MIME_SWF;
    }
    else if(strcmp(ptr, ".js") == 0) {
        format = MIME_APPLICATION_JAVASCRIPT;
    }
    else {
        format = MIME_TEXT_HTML;
    }


    char * date = http_date(resultFile->date);


    data = NULL;
    dataLen = NULL;
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

ResponseData::ResponseData(RequestData * request) {

    countHeaderSendedBytes = 0;
    countDataSendedBytes = 0;
    this->request = request;
    header = (char *) malloc(MAX_RESPONSE_HEADER_LEN);
    getHTTPResponse();
}

void ResponseData::clear() {
    countHeaderSendedBytes = 0;
    countDataSendedBytes = 0;
}

ResponseData::~ResponseData(){
    if (header != NULL) free(header);
    if (data != NULL) free(data);

}