#pragma once

struct Field {
    s32 mFieldHash; //_0
    s32 mDefaultValue;  //_4
    u16 mShift;   //_8
    u8 mUnk;  //_A
    u8 mDataType; //_B
};

class BCSV {
public:

    // read functions

    static BCSV* loadBcsv(const char* pFileName);

    bool isEndianCorrect();
    void swapEndian();
    void swapEndianEntry(u32 Index);

    const Field* getFieldInfo(const char* FieldName) const;
    
    u32 findElementString(const char* FieldName, const char* ValueToSearchFor) const;
    template<typename T>
    u32 findElement(const char* FieldName, T ValueToSearchFor) const;
    bool getValue(u32 Index, const char* FieldName, void* ValuePtr) const;
    bool getValueFast(u32 Index, const Field*, void* ValuePtr) const;

    static s32 calcHash(const char*);



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

    static BCSV* createNewBcsv(u32 EntryNum);
    void addField(const char* FieldName, DataType type);
    u16 calcShift();
    static u8 getDataSize(u8 type);
    void writeEntry(u32 Index, const char* FieldName, const void* value);
    u32 tryAddString(const char* str);
    u32 getFileSizeAndAddPadding();
    void sortAlphabet(const char* pFieldName);


    u32 mEntryNum;
    u32 mFieldNum;
    u32 mDataAdr;
    u32 mEntrySize;
    Field mFields[];

    // special functions for sorting the bcsv
private:
    static bool isStringInAlphabetBefore(const char* pStringToCheck, const char* pSrcString);
};