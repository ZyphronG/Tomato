#include "XML.h"
#include <iostream>
#include <cstring>


unsigned long XmlUtil::getNumberOfEntries(const char* pEntryName, FileHolder* pXmlFile) {
    XMLEntry XmlEntry = XMLEntry(pXmlFile);
    unsigned long XmlOffset = 0;
    unsigned long EntryCount = 0;

    while (true) {
		if (!XmlEntry.findEntry(pEntryName, XmlOffset, false))
			break;

		if (XmlEntry.mEntryStart)
			XmlOffset = XmlEntry.mEntryEnd - pXmlFile->mFileData;
		else
			XmlOffset++;

        EntryCount++;
	}

    return EntryCount;
}

bool XmlUtil::getEntryByIndex(const char* pEntryName, unsigned long pIndex, FileHolder* pXmlFile, XMLEntry* pXmlEntry) {
    XMLEntry XmlEntry = XMLEntry(pXmlFile);
    unsigned long XmlOffset = 0;
    unsigned long EntryCount = 0;

    while (true) {
		if (!XmlEntry.findEntry(pEntryName, XmlOffset, false))
			return false;

		if (XmlEntry.mEntryStart) {
            if (EntryCount == pIndex)
                break;
			XmlOffset = XmlEntry.mEntryEnd - pXmlFile->mFileData;
        }
		else
			XmlOffset++;

        EntryCount++;
	}

    memcpy(pXmlEntry, &XmlEntry, sizeof(XMLEntry));
    return true;
}

unsigned long XmlUtil::countLines(const char* pStartString, const char* pEndString) {
    unsigned long LineCount = 1;

    while (pStartString < pEndString) {
        if (*pStartString == '\n')
            LineCount++;
        
        pStartString++;
    }

    return LineCount;
}

bool XmlUtil::isValidStringEndChar(char pChar) {
    if (pChar == ' ' || pChar == '\t' || pChar == '\n' || pChar == '\r')
        return true;
    else
        return false;
}

void XmlUtil::extractStringFromQuotes(const char* pInput, char* pOutput) {
    pInput = strstr(pInput, "\"") + 1;
    
    while (*pInput != '\"') {
        *pOutput = *pInput;
        pInput++;
        pOutput++;
    }

    *pOutput = 0;
}

bool XmlUtil::isEqualEntryName(const char* pEntryName, const char* pXmlEntry) {
    const char* Src = pXmlEntry + 1;

    while (true) {
        if (XmlUtil::isValidStringEndChar(*Src) || *Src == '/' || *Src == '>') {
            if (*pEntryName == 0)
                return true;
        }

        if (*Src != *pEntryName)
            return false;

        Src++;
        pEntryName++;
    }
}

long XmlUtil::getEntryIndexByEntryNameAndInfo(const char* pEntryName, const char* pInfoName, const char* pInfoValue, FileHolder* pXmlFile) {
    XMLEntry XmlEntry = XMLEntry(pXmlFile);
    char Value[256];
    long IndexCounter = 0;

    while (XmlEntry.getNextEntry(pEntryName)) {
        XmlEntry.getEntryInfo(pInfoName, Value);
        if (strcmp(Value, pInfoValue) == 0)
            return IndexCounter;

        IndexCounter++;
    }

    return -1;
}











XMLEntry::XMLEntry() {
    mXmlFile = 0;
    reset();
}

XMLEntry::XMLEntry(FileHolder* pFileHolder) {
    mXmlFile = pFileHolder;
    reset();
}



bool XMLEntry::findEntry(const char* pEntryName, unsigned long pOffset, bool pOnlySearchForEntryStart) {
    mEntryName = pEntryName;
    mEntryStart = 0;
    mEntryEnd = 0;
    mIsLongEntry = false;

    bool isSearchForEntryEnd = false;
    bool isFoundEntryStart = false;
    bool isInQuote = false;
    bool isInComment = false;

    char* endOfXml = mXmlFile->mFileData + mXmlFile->mFileSize;
    char* XmlFileParser = mXmlFile->mFileData + pOffset;
    size_t EntryNameLength = strlen(mEntryName);
    char* entryStart;
    const char* entryNameParser;


    while (XmlFileParser < endOfXml) {
        // toggle is in comment
        if (strncmp(XmlFileParser, "<!--", 4) == 0)
            isInComment = true;

        if (strncmp(XmlFileParser, "-->", 3) == 0)
            isInComment = false;

        // toggle in quote
        if (*XmlFileParser == '\"' && !isInComment) {
            isInQuote = !isInQuote;
            isFoundEntryStart = false;
        }

        if (!isInQuote && !isInComment) {

            if (*XmlFileParser == '<') {
                // toggle because if we find an entry start then this is
                // probably not an actual entry that has been found so far
                isFoundEntryStart = true;
                isSearchForEntryEnd = false;
                entryStart = XmlFileParser;
                XmlFileParser++;
                entryNameParser = mEntryName;
            }
                
            // check if this entry has the correct entry name
            if (isFoundEntryStart) {
                if (*XmlFileParser != *entryNameParser) {
                    // check if the parsed string length equals the EntryName's length
                    if (entryStart + EntryNameLength == XmlFileParser - 1) {
                        // short entry
                        if (*XmlFileParser == '>' || (XmlFileParser[0] == '/' && XmlFileParser[1] == '>') )
                            goto writeValues;
                        // long entry with extra info or just some extra spaces
                        if (XmlUtil::isValidStringEndChar(*XmlFileParser))
                            isSearchForEntryEnd = true;
                    }
                    isFoundEntryStart = false;
                }

                entryNameParser++;
            }

            if (isSearchForEntryEnd) {
                if (*XmlFileParser == '>' || (XmlFileParser[0] == '/' && XmlFileParser[1] == '>') )
                    goto writeValues;
            }
        }

        XmlFileParser++;
    }

    return false;

    

    writeValues:

    if (!pOnlySearchForEntryStart) {
        if (*XmlFileParser == '>') {
            char* EntryEnd = getEndOfLongEntry(XmlFileParser);
            if (!EntryEnd)
                return false;

            mEntryEnd = EntryEnd;
            mIsLongEntry = true;
        }
        else
            mEntryEnd = XmlFileParser + 2;
    }

    mEntryStart = entryStart;

    return true;
}

char* XMLEntry::getEndOfLongEntry(char* pXmlParser) {
    char EntryEndName[128];
    XMLEntry Entry = XMLEntry(mXmlFile);

    sprintf(EntryEndName, "/%s", mEntryName);

    if (!Entry.findEntry(EntryEndName, pXmlParser - mXmlFile->mFileData, true))
        return 0;

    return Entry.mEntryStart;
}


bool XMLEntry::getEntryInfo(const char* pEntryInfoName, char* pOutputString) {
    unsigned long EntryInfoNameParser = 0;
    unsigned long EntryInfoNameLength = strlen(pEntryInfoName);
    char* XmlFileParser = mEntryStart + strlen(mEntryName) + 1;

    bool isInQuote = false;

    while (XmlFileParser && XmlFileParser < mEntryEnd) {
        // toggle in quote
        if (*XmlFileParser == '\"') {
            isInQuote = !isInQuote;
            EntryInfoNameParser = 0;
        }

        if (!isInQuote) {
            if (*XmlFileParser == '>')
                return false;

            if (EntryInfoNameParser == EntryInfoNameLength && *XmlFileParser == '=') {
                XmlUtil::extractStringFromQuotes(XmlFileParser, pOutputString);
                return true;
            }

            if (*XmlFileParser == pEntryInfoName[EntryInfoNameParser]) {
                if ( (EntryInfoNameParser == 0 && XmlUtil::isValidStringEndChar(*(XmlFileParser-1))) || EntryInfoNameParser > 0)
                    EntryInfoNameParser++;
            }
            else
                EntryInfoNameParser = 0;
        }

        XmlFileParser++;
    }

    return false;
}

bool XMLEntry::getNextEntry(const char* pEntryName) {
    if (mEntryEnd)
        return findEntry(pEntryName, mEntryEnd - mXmlFile->mFileData, false);
    else
        return findEntry(pEntryName, 0, false);
}

char* XMLEntry::getEntryData() const {
    if (mIsLongEntry) {
        char* XmlFileParser = mEntryStart + strlen(mEntryName) + 1;
        bool isInQuote = false;

        while (XmlFileParser && XmlFileParser < mEntryEnd) {
            // toggle in quote
            if (*XmlFileParser == '\"') {
                isInQuote = !isInQuote;
            }

            if (!isInQuote) {
                if (*XmlFileParser == '>')
                    return XmlFileParser + 1;

                if (*XmlFileParser == '<')
                    return 0;
            }

            XmlFileParser++;
        }

        return 0;
    }
    else
        return 0;
}

bool XMLEntry::getEntry(const char* pEntryName, const char* pXmlEntry) {
    if (XmlUtil::isEqualEntryName(pEntryName, pXmlEntry)) {
        if (findEntry(pEntryName, pXmlEntry - mXmlFile->mFileData, false) && mEntryStart == pXmlEntry)
            return true;
    }
        
    reset();
    return false;
}

void XMLEntry::reset() {
    mEntryName = 0;
    mEntryStart = 0;
    mEntryEnd = 0;
    mIsLongEntry = false;
}


XMLSubEntry::XMLSubEntry() : XMLEntry() {
    mXmlParentEntry = 0;
}

XMLSubEntry::XMLSubEntry(const XMLEntry* pXmlParentEntry) : XMLEntry(pXmlParentEntry->mXmlFile) {
    mXmlParentEntry = pXmlParentEntry;
}

bool XMLSubEntry::findSubEntry(const char* pEntryName) {
    if (mXmlParentEntry->mIsLongEntry && findEntry(pEntryName, mXmlParentEntry->mEntryStart - mXmlParentEntry->mXmlFile->mFileData, false) && mEntryStart < mXmlParentEntry->mEntryEnd)
        return true;
    else {
        reset();
        return false;
    }
}

bool XMLSubEntry::findSubEntry(const char* pEntryName, unsigned long pOffset) {
    if (mXmlParentEntry->mIsLongEntry && findEntry(pEntryName, (mXmlParentEntry->mEntryStart + pOffset) - mXmlParentEntry->mXmlFile->mFileData, false) && mEntryStart < mXmlParentEntry->mEntryEnd)
        return true;
    else {
        reset();
        return false;
    }
}