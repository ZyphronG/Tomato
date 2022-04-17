#include "../include/FileHolder.h"
#include <iostream>
#include <fstream>

FileHolder::FileHolder(const char* pFileName) {
    mFileName = pFileName;
    mFileData = 0;
    mFileSize = 0;
}

bool FileHolder::loadFile() {
    std::ifstream in;

	in.open(mFileName, std::ios::in | std::ios::binary | std::ios::ate);

    if (!in.is_open())
        return false;

    // since we set the initial position at the end of the file (ios::ate)
	// we can easily determine the filesize by getting the current streampos of the fstream
	mFileSize = in.tellg();

    // reset the streampos to the start of the file
	in.seekg(0, std::ios::beg);

    // allocate memory for the file and add an 0x0 byte at the end of the data
	mFileData = new char[mFileSize+1];
    mFileData[mFileSize] = 0;

	// read the file's contents into the temprary char array for the BMG file
	in.read(mFileData, mFileSize);
	in.close();

    return true;
}