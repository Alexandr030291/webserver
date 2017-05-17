#include "filemanager.h"

FileData* getFile(char * path, char * dir_path, bool readFile) {

    FileData *resultFile = new FileData;
    FILE *fp;
    int path_len = (int) strlen(path);
    int dir_path_len = (int) strlen(dir_path);
    if (path_len + dir_path_len >= 512) {
        return NULL;
    }

    char name[512];
    memcpy(name, dir_path, strlen(dir_path));
    memcpy(name + strlen(dir_path), path, (size_t) (path_len + 1));
    fp = fopen(name, "rb");
    if (fp != NULL && (strchr(path, '.') >= 0)) {

        struct stat myStat;
        stat(name, &myStat);
        char *buf = NULL;

        if (readFile) {
            buf = (char *) malloc((size_t) (myStat.st_size + 25));
            fread(buf, 1, (size_t) myStat.st_size, fp);
            fclose(fp);
        }

        resultFile->data = buf;
        resultFile->length = (unsigned long) myStat.st_size;
        resultFile->date = gmtime(&myStat.st_mtim.tv_sec);
        resultFile->success = true;
    } else {
        resultFile->data = NULL;
        resultFile->date = NULL;
        resultFile->length = 0;
        resultFile->success = false;
    }

    return resultFile;
}