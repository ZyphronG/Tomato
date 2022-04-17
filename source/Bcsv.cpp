#include <fstream>
#include <iostream>
#include <cstring>
#include "../include/Bcsv.h"

#define BCSV_MAX_ALOC_SIZE 0x80000

using namespace std;

BCSV* BCSV::loadBcsv(const char* pFileName) {
    ifstream inFile;
	streampos size;
    char* returnBCSV;

	inFile.open(pFileName, ios::in | ios::binary | ios::ate);

    if (inFile.is_open()) {
        size = inFile.tellg();
        inFile.seekg(0, ios::beg);
        returnBCSV = new char[size];
        inFile.read(returnBCSV, size);
		inFile.close();

        BCSV* bcsv = (BCSV*)returnBCSV;
        if (!bcsv->isEndianCorrect())
            bcsv->swapEndian();

        return bcsv;
    }

    return 0;
}


// detect if the current Endian is correct. A bit jank and unreliable but meh
// loads the FieldNum and if it's < 0x10000 it's probably the right endian
bool BCSV::isEndianCorrect() {
    if (mFieldNum < 0x10000)
        return true;
    else
        return false;
}

void BCSV::swapEndian() {
    u32 EntryNumTmp = mEntryNum;
    u32 FieldNumTmp = mFieldNum;

    mEntryNum = _byteswap_ulong(mEntryNum);
	mFieldNum = _byteswap_ulong(mFieldNum);
	mDataAdr = _byteswap_ulong(mDataAdr);
	mEntrySize = _byteswap_ulong(mEntrySize);

    if (isEndianCorrect()) {
        EntryNumTmp = mEntryNum;
        FieldNumTmp = mFieldNum;
    }

    for (u32 i = 0; i < FieldNumTmp; i++) {
        Field* FieldTmp = &mFields[i];
        FieldTmp->mFieldHash = _byteswap_ulong(FieldTmp->mFieldHash);
        FieldTmp->mDefaultValue = _byteswap_ulong(FieldTmp->mDefaultValue);
        FieldTmp->mShift = _byteswap_ushort(FieldTmp->mShift);
    }

    for (u32 i = 0; i < EntryNumTmp; i++)
        swapEndianEntry(i);
}

void BCSV::swapEndianEntry(u32 Index) {
    u32 FieldNumTmp = mFieldNum;
    u32 DataAdrTmp = mDataAdr;
    u32 EntrySizeTmp = mEntrySize;

    if (!isEndianCorrect()) {
        FieldNumTmp = _byteswap_ulong(FieldNumTmp);
        DataAdrTmp = _byteswap_ulong(DataAdrTmp);
        EntrySizeTmp = _byteswap_ulong(EntrySizeTmp);
    }

    char* CurrDataAddress = (char*)this + DataAdrTmp + Index * EntrySizeTmp;
    
    for (u32 i = 0; i < FieldNumTmp; i++) {
        u16 ShiftTmp;
        u16* CurrDataAddressShort;
        u32* CurrDataAddressLong;

        if (!isEndianCorrect())
            ShiftTmp = _byteswap_ushort(mFields[i].mShift);
        else
            ShiftTmp = mFields[i].mShift;

        switch (mFields[i].mDataType) {
            // string / byte
            case String:
            case Char:
                break;
            // short
            case Short:
                CurrDataAddressShort = (u16*)(CurrDataAddress + ShiftTmp);
                *CurrDataAddressShort = _byteswap_ushort(*CurrDataAddressShort);
                break;
            // long, float, string offset
            default:
                CurrDataAddressLong = (u32*)(CurrDataAddress + ShiftTmp);
                *CurrDataAddressLong = _byteswap_ulong(*CurrDataAddressLong);
                break;
        }
    }
}

const Field* BCSV::getFieldInfo(const char* FieldName) const {
    s32 Hash = calcHash(FieldName);

    for (u32 i = 0; i < mFieldNum; i++) {
        if (mFields[i].mFieldHash == Hash) 
            return &mFields[i];
    }

    return 0;
}

u32 BCSV::findElementString(const char* FieldName, const char* ValueToSearchFor) const {
    const Field* field = getFieldInfo(FieldName);
    const char* InputTemp;

    if (!field)
        return mEntryNum;

    for (u32 i = 0; i < mEntryNum; i++) {

        if (!getValueFast(i, field, &InputTemp))
            return mEntryNum;  // returnd false if the data type couldn't be found
        
        if (strcmp(InputTemp, ValueToSearchFor) == 0)
            return i;
    }

    return mEntryNum;
}

template<typename T>
u32 BCSV::findElement(const char* FieldName, T ValueToSearchFor) const {
    const Field* field = getFieldInfo(FieldName);
    T InputTemp = 0;

    if (!field)
        return mEntryNum;

    for (u32 i = 0; i < mEntryNum; i++) {

        if (!getValueFast(i, field, &InputTemp))
            return mEntryNum;  // returnd false if the data type couldn't be found
        
        if (InputTemp == ValueToSearchFor)
            return i;
    }

    return mEntryNum;
}

bool BCSV::getValue(u32 Index, const char* FieldName, void* ValuePtr) const {
    const Field* field = getFieldInfo(FieldName);

    if (Index < mEntryNum && field && getValueFast(Index, field, ValuePtr))
        return true;
    else
        return false;
}

bool BCSV::getValueFast(u32 Index, const Field* pField, void* ValuePtr) const {
    char* ValueAdr = (char*)this + mDataAdr + Index * mEntrySize + pField->mShift;

    switch (pField->mDataType) {
        // long
        case Long:
        case _Long: {
            s32* ReturnVal = (s32*)ValuePtr;
            *ReturnVal = *(s32*)ValueAdr;
            return true;
        }

        // string
        case String: {
            const char** ReturnVal = (const char**)ValuePtr;
            *ReturnVal = (const char*)ValueAdr;
            return true;
        }

        // float
        case Float: {
            float* ReturnVal = (float*)ValuePtr;
            *ReturnVal = *(float*)ValueAdr;
            return true;
        }

        // short
        case Short: {
            s16* ReturnVal = (s16*)ValuePtr;
            *ReturnVal = *(s16*)ValueAdr;
            return true;
        }

        // char
        case Char: {
            char* ReturnVal = (char*)ValuePtr;
            *ReturnVal = *(char*)ValueAdr;
            return true;
        }

        // string offset
        case StringOffset: {
            const char** ReturnVal = (const char**)ValuePtr;
            *ReturnVal = (const char*)this + mDataAdr + mEntryNum*mEntrySize + *(u32*)ValueAdr;
            return true;
        }

        default: {
            return false;
        }
    }
}

s32 BCSV::calcHash(const char* InputStr) {
    char CurrChar;
    s32 Output;
  
    Output = 0;
    while(true) {
        CurrChar = *InputStr;
        if (CurrChar == 0)
            break;
        InputStr += 1;
        Output = CurrChar + Output * 0x1f;
    }

    return Output;
}









BCSV* BCSV::createNewBcsv(u32 EntryNum) {
    //allocate 500 KB for the creation of the bcsv and clean it
    char* bcsvInit = new char[BCSV_MAX_ALOC_SIZE];
    BCSV* returnBcsv = (BCSV*)bcsvInit;

    // clean allocated memory
    memset(bcsvInit, 0, BCSV_MAX_ALOC_SIZE);

    returnBcsv->mEntryNum = EntryNum;
    returnBcsv->mDataAdr = 0x10;

    return returnBcsv;
}

void BCSV::addField(const char* FieldName, DataType type) {
    mFields[mFieldNum].mFieldHash = calcHash(FieldName);

    switch (type) {
        case Short:
            mFields[mFieldNum].mDefaultValue = 0xFFFF;
            break;
        case Char:
            mFields[mFieldNum].mDefaultValue = 0xFF;
            break;
        default:
            mFields[mFieldNum].mDefaultValue = -1;
            break;
    }
    
    mFields[mFieldNum].mShift = calcShift();
    // if the data type is a float make sure to add padding to avoid real hardware crashes
    while (mFields[mFieldNum].mShift % 0x4 != 0)
        mFields[mFieldNum].mShift++;
    
    mFields[mFieldNum].mDataType = type;

    mDataAdr += sizeof(Field);
    mEntrySize += getDataSize(type);
    mFieldNum++;
}

u16 BCSV::calcShift() {
    // get the shift from the field before (if there exists a field before the current one)
    // and add the size of the data type
    if (mFieldNum > 0)
        return mFields[mFieldNum-1].mShift + getDataSize(mFields[mFieldNum-1].mDataType);

    return 0;
}

u8 BCSV::getDataSize(u8 type) {
    switch (type) {
    case String:
        return 0x20;
    case Short:
        return 0x2;
    case Char:
        return 0x1;
    default:
        return 0x4;
    }
}


void BCSV::writeEntry(u32 Index, const char* FieldName, const void* value) {
    const Field* field = getFieldInfo(FieldName);

    if (!field) {
        std::cout << "The Field " << FieldName << " was not found!\nMake sure to add all necessary fields before writing Entries.";
        return;
    }

    char* ValueAdr = (char*)this + mDataAdr + Index * mEntrySize + field->mShift;

    switch (field->mDataType) {
    case Long:
    case _Long: {
        s32* ValueAdrLong = (s32*)ValueAdr;
        *ValueAdrLong = *(s32*)value;
        break;
        }
    case String: {
        strncpy(ValueAdr, (const char*)value, 0x20);
        break;
        }
    case Float: {
        float* ValueAdrFloat = (float*)ValueAdr;
        *ValueAdrFloat = *(float*)value;
        break;
        }
    case Short: {
        s16* ValueAdrShort = (s16*)ValueAdr;
        *ValueAdrShort = *(s16*)value;
        break;
        }
    case Char: {
        *ValueAdr = *(char*)value;
        break;
        }
    case StringOffset: {
        u32* ValueAdrULong = (u32*)ValueAdr;
        *ValueAdrULong = tryAddString((const char*)value);
        break;
        }
    }
}

u32 BCSV::tryAddString(const char* str) {
	u32 i = 0;
    char* StringTable = (char*)this + mDataAdr + mEntryNum * mEntrySize;
    // hidden StringTable watcher at the end of the allocated memory
    u32* StringTableWatcher = (u32*)((char*)this + BCSV_MAX_ALOC_SIZE - 0x4);

	while (true) {
		// check if we reached the current end of the string table
		if (i >= *StringTableWatcher) {
			strcpy(StringTable + i, str);
            *StringTableWatcher += strlen(StringTable + i) + 1;
			break;
		}

		if (strcmp(StringTable + i, str) == 0)
			break;
        
		i += strlen(StringTable + i) + 1;
	}

	return i;
}

u32 BCSV::getFileSizeAndAddPadding() {
    char* StringTable = (char*)this + mDataAdr + mEntryNum * mEntrySize;
    u32 Size = 0;
    u32* StringTableWatcher = (u32*)((char*)this + BCSV_MAX_ALOC_SIZE - 0x4);

    Size = (StringTable + *StringTableWatcher) - (char*)this;

    // add Padding
    while (Size % 0x20 != 0) {
        *((char*)this + Size) = '@';
        Size++;
    }

    return Size;
}

void BCSV::sortAlphabet(const char* pFieldName) {
    const Field* field = getFieldInfo(pFieldName);

    if (!field)
        return;

    // set up storage
    char* fieldStorage = new char[mEntryNum*mEntrySize];
    memcpy(fieldStorage, (char*)this + mDataAdr, mEntryNum*mEntrySize);

    // set up string container
    char** Strings = new char*[mEntryNum];
    for (u32 i = 0; i < mEntryNum; i++)
        getValueFast(i, field, &Strings[i]);

    for (u32 i = 0; i < mEntryNum; i++) {
        const char* CurrentHighestString = 0;
        u32 StringIter = 0;
        // get next string to check
        while (!CurrentHighestString) {
            CurrentHighestString = Strings[StringIter];
            StringIter++;
        }

        // iterate through all strings
        for (u32 f = 0; f < mEntryNum; f++) {
            const char* CurrentString = Strings[f];
            if (CurrentString && isStringInAlphabetBefore(CurrentString, CurrentHighestString)) {
                StringIter = f;     // store this so we can easily retreive the field entry ID as well as the string itself
                CurrentHighestString = CurrentString;
            }
        }

        // remove current highest string from array
        Strings[StringIter] = 0;
        // copy entry to the right spot in the bcsv
        memcpy((char*)this + mDataAdr + i*mEntrySize, fieldStorage + StringIter*mEntrySize, mEntrySize);
    }

    delete[] Strings;
    delete[] fieldStorage;
}


bool BCSV::isStringInAlphabetBefore(const char* pStringToCheck, const char* pSrcString) {
    while (true) {
        if (*pStringToCheck > *pSrcString)
            return false;
        else if (*pStringToCheck < *pSrcString)
            return true;

        if (*pStringToCheck == 0)
            return true;

        pStringToCheck++;
        pSrcString++;
    }
}