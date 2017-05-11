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



