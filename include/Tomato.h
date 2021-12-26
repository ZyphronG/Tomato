#pragma once

#include <fstream>
#include "MESGbmg1.h"
#include "Bcsv.h"
#include "XML.h"
#include "FileHolder.h"

class BmgFileHolder {
public:
	BmgFileHolder(const char* FileName);

	bool loadBmg();

	FileHolder mFileHolder;
	MESGbmg1::FileHeader* Header;
	MESGbmg1::INF1* InfoSection;
	MESGbmg1::DAT1* DataSection;
	MESGbmg1::FLW1* FlowSection;
	bool isSwapEndian;
	bool isBmgBigEndian;
};

class XmlWriter {
public:
	XmlWriter(const char* pXmlFileName, const BmgFileHolder* pBMG, const BCSV* pBCSV);
	

	// XML writing tools
	bool writeXml();
	const char* getStringFromBcsv(unsigned long Index);
	void createAndWriteMessage(unsigned long Index);
	unsigned long createEventText(unsigned long TextOffset);
	bool isNeedWriteFlowSection(unsigned long pMessageId);
	void findNodeByMessageID(unsigned long pMessageId);
	void writeFlows(unsigned long pNodeID, bool pIsInContainer);

	const char* FileName;
	const BmgFileHolder* BMG;
	const BCSV* bcsv;
	char* ConvertedFlowStorage;
	std::ofstream outFile;
};

class BmgTextGenerator {
public:
	BmgTextGenerator(const XMLEntry* pXmlParentEntry);

	bool createMessage();
	void createAndAddEventData(const char* pXmlEntry);
	void writeChar(wchar_t pChar);
	void writeEvent(unsigned char pEventType, unsigned char pLength);
	void writeEventLength(unsigned long pEventStartOffset);
	void writeSingleChar(unsigned char pChar, bool pSide = false);
	void writeLong(long pLong);

	XMLSubEntry mXmlSubEntry;
	unsigned long mMessagePos;
	wchar_t mMessageData[0x10000];
};

class BmgWriter {
public:
	BmgWriter(const char* pXmlFileName, const char* pBmgFileName, const char* pTblFileName);

	bool writeBmg(bool pIsLe);
	unsigned long tryAddNewTextData(const wchar_t* pTextData, unsigned long pTextLength);
	bool generateTextNode(MESGbmg1::NodeEntryText* pTextNode, XMLEntry& pXmlEntry);
	bool generateConditionNode(MESGbmg1::NodeEntryCondition* pConNode, XMLEntry& pXmlEntry);
	bool generateEventNode(MESGbmg1::NodeEntryEvent* pEventNode, XMLEntry& pXmlEntry);
	void addBranchNode(unsigned long pFlowEntryID);
	bool writeMissingErrorMessage(const char* pMissingInfoName, char* pXmlEntryStart);
	bool writeInvalidErrorMessage(const char* pMissingInfoName, char* pXmlEntryStart);

	const char* mTblFileName;
	BmgFileHolder* BMG;
	BCSV* bcsv;
	FileHolder* XML;
	std::ofstream outFile;
};