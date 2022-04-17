#pragma once

#include "FileHolder.h"
#include "Types.h"


class XMLEntry {
public:
    XMLEntry();
    XMLEntry(FileHolder* pFileHolder);

    bool findEntry(const char* pEntryName, u32 pOffset, bool pOnlySearchForEntryStart = false);
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
    bool findSubEntry(const char* pEntryName, u32 pOffset);

    const XMLEntry* mXmlParentEntry;
};

namespace XmlUtil {
    u32 getNumberOfEntries(const char* pEntryName, FileHolder* pXmlFile);
    bool getEntryByIndex(const char* pEntryName, u32 pIndex, FileHolder* pXmlFile, XMLEntry* pXmlEntry);
    u32 countLines(const char* pStartString, const char* pEndString);
    bool isValidStringEndChar(char);
    void extractStringFromQuotes(const char* pInput, char* pOutput);
    bool isEqualEntryName(const char* pEntryName, const char* pXmlEntry);
    s32 getEntryIndexByEntryNameAndInfo(const char* pEntryName, const char* pInfoName, const char* pInfoValue, FileHolder* pXmlFile);
}