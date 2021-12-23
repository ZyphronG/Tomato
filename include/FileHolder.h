#pragma once

#include <cstddef>

class FileHolder {
public:
    FileHolder() {}
    FileHolder(const char* pFileName);

    bool loadFile();

    const char* mFileName;
    char* mFileData;
    size_t mFileSize;
};