#pragma once

#include "FileHolder.h"


class XMLEntry {
public:
    XMLEntry();
    XMLEntry(FileHolder* pFileHolder);

    bool findEntry(const char* pEntryName, unsigned long pOffset, bool pOnlySearchForEntryStart);
    char* getEndOfLongEntry(char* pXmlParser);
    bool getEntryInfo(const char* pEntryInfoName, char* pOutputString);
    bool getNextEntry(const char* pEntryName);
    char* getEntryData() const;
    bool getEntry(const char* pEntryName, const char* pXmlEntry);
    void reset();

    FileHolder* mXmlFile;
    const char* mEntryName;
    char* mEntryStart;
    char* mEntryEnd;
    bool mIsLongEntry;
};

class XMLSubEntry : public XMLEntry {
public:
    XMLSubEntry();
    XMLSubEntry(const XMLEntry* pXmlParentEntry);

    bool findSubEntry(const char* pEntryName);
    bool findSubEntry(const char* pEntryName, unsigned long pOffset);

    const XMLEntry* mXmlParentEntry;
};

namespace XmlUtil {
    unsigned long getNumberOfEntries(const char* pEntryName, FileHolder* pXmlFile);
    bool getEntryByIndex(const char* pEntryName, unsigned long pIndex, FileHolder* pXmlFile, XMLEntry* pXmlEntry);
    unsigned long countLines(const char* pStartString, const char* pEndString);
    bool isValidStringEndChar(char);
    void extractStringFromQuotes(const char* pInput, char* pOutput);
    bool isEqualEntryName(const char* pEntryName, const char* pXmlEntry);
    long getEntryIndexByEntryNameAndInfo(const char* pEntryName, const char* pInfoName, const char* pInfoValue, FileHolder* pXmlFile);
}