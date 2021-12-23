#pragma once

struct Field {
    long mFieldHash; //_0
    long mDefaultValue;  //_4
    unsigned short mShift;   //_8
    unsigned char mUnk;  //_A
    unsigned char mDataType; //_B
};

class BCSV {
public:

    // read functions

    static BCSV* loadBcsv(const char* pFileName);

    bool isEndianCorrect();
    void swapEndian();
    void swapEndianEntry(unsigned long Index);

    const Field* getFieldInfo(const char* FieldName) const;
    
    unsigned long findElementString(const char* FieldName, const char* ValueToSearchFor) const;
    template<typename T>
    unsigned long findElement(const char* FieldName, T ValueToSearchFor) const;
    bool getValue(unsigned long Index, const char* FieldName, void* ValuePtr) const;
    bool getValueFast(unsigned long Index, const Field*, void* ValuePtr) const;

    static long calcHash(const char*);



    // write functions

    enum DataType {
        Long = 0,
        String = 1,     // not recommended, use StringOffset
        Float = 2,
        _Long = 3,      // same as 0?
        Short = 4,
        Char = 5,
        StringOffset = 6
    };

    static BCSV* createNewBcsv(unsigned long EntryNum);
    void addField(const char* FieldName, DataType type);
    unsigned short calcShift();
    static unsigned char getDataSize(unsigned char type);
    void writeEntry(unsigned long Index, const char* FieldName, const void* value);
    unsigned long tryAddString(const char* str);
    unsigned long getFileSizeAndAddPadding();
    void sortAlphabet(const char* pFieldName);


    unsigned long mEntryNum;
    unsigned long mFieldNum;
    unsigned long mDataAdr;
    unsigned long mEntrySize;
    Field mFields[];

    // special functions for sorting the bcsv
private:
    static bool isStringInAlphabetBefore(const char* pStringToCheck, const char* pSrcString);
};