#pragma once
#ifndef WEBSERVER_FILEMANAGER_H
#define WEBSERVER_FILEMANAGER_H

#include <stdio.h>
#include <memory.h>
#include <malloc.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

struct FileData {
    unsigned long length;
    char * data;
    tm * date;
    bool success;
};

FileData* getFile(char * path, char * dir_path, bool readFile = true);

#endif //WEBSERVER_FILEMANAGER_H
