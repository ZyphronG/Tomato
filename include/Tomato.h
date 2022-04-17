#pragma once

#include <fstream>
#include "MESGbmg1.h"
#include "Bcsv.h"
#include "XML.h"
#include "FileHolder.h"
#include "Types.h"

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
	const char* getStringFromBcsv(u32 Index);
	void createAndWriteMessage(u32 Index);
	u32 createEventText(u32 TextOffset);
	bool isNeedWriteFlowSection(u32 pMessageId);
	void findNodeByMessageID(u32 pMessageId);
	void writeFlows(u32 pNodeID, bool pIsInContainer);

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
	void writeChar(c16 pChar);
	void writeEvent(u8 pEventType, u8 pLength);
	void writeEventLength(u32 pEventStartOffset);
	void writeSingleChar(u8 pChar, bool pSide = false);
	void writeLong(s32 pLong);

	XMLSubEntry mXmlSubEntry;
	u32 mMessagePos;
	bool mIsRandomActive;
	c16 mMessageData[0x10000];
};

class BmgWriter {
public:
	BmgWriter(const char* pXmlFileName, const char* pBmgFileName, const char* pTblFileName);

	bool writeBmg(bool pIsLe, u32 RandSeed);
	u32 tryAddNewTextData(const c16* pTextData, u32 pTextLength);
	bool generateTextNode(MESGbmg1::NodeEntryText* pTextNode, XMLEntry& pXmlEntry);
	bool generateConditionNode(MESGbmg1::NodeEntryCondition* pConNode, XMLEntry& pXmlEntry);
	bool generateEventNode(MESGbmg1::NodeEntryEvent* pEventNode, XMLEntry& pXmlEntry);
	void addBranchNode(u32 pFlowEntryID);
	bool writeMissingErrorMessage(const char* pMissingInfoName, char* pXmlEntryStart);
	bool writeInvalidErrorMessage(const char* pMissingInfoName, char* pXmlEntryStart);
	void randomize(u32 Seed);

	const char* mTblFileName;
	BmgFileHolder* BMG;
	BCSV* bcsv;
	FileHolder* XML;
	std::ofstream outFile;
	u8 mRandomType;
};



class Randomizer {
public:
	Randomizer(BmgWriter* pBmgWrt, u32 Seed);

	void randomize(const c8** pNameList);
	bool canUse(const c8* pName, const c8** pNameList);
	u32 getRandom(u32 range);
	
	u32 mRandomSeed;
	bool mIsNoLogic;
	BmgWriter* mBmgWrt;
	s32 mIterNum;
	u32* mSrc;
	bool* mIsUsedAlready;
	bool* mCanBeUsed;
};


// Random List

const c8* ScenarioAndGalaxy[] {
	"ScenarioName_",
	"GalaxyNameShort_",
	"GalaxyName_",
	"-Layout_PrologueDemoPro",
	"-Layout_PeachLetter",
	"-Layout_StoryDemo",
	"Layout_",
	"System_FileSelect",
	"System_Date000",
	0
};

const c8* ChoiceNames[] {
	"System_Save07_No",
	"System_Save07_Yes",
	"ConfirmEndGame_No",
	"ConfirmEndGame_Yes",

	"NAND_02_No",
	"NAND_02_Yes",
	"NAND_03_No",
	"NAND_03_Yes",
	"NAND_07_No",
	"NAND_07_Yes",

	"PauseMenu_ButtonEndGame",
	"PauseMenu_ConfirmBackAstroDome_No",
	"PauseMenu_ConfirmBackAstroDome_Yes",
	"PauseMenu_ConfirmEndGame_No",
	"PauseMenu_ConfirmEndGame_Yes",

	"Select_",

	"System_Save00_No",
	"System_Save00_Yes",
	"System_Save03_No",
	"System_Save03_Yes",
	"System_Save04_No",
	"System_Save04_Yes",
	"System_Save07_No",
	"System_Save07_Yes",

	0
};

const c8** NameList[] {
	ScenarioAndGalaxy,
	ChoiceNames,
	0
};

const c8* NameListNoLogic[] {
	0
};


// exception table, mostly PictureFont only text

const c8* ExceptionNameTable[] {
	// pointer number
	"Layout_BatteryInfoP1",
	"Layout_BatteryInfoP2",
	"Layout_DPDPointerNumberP1",
	"Layout_DPDPointerNumberP2",
	"Layout_P2ManualP1",
	"Layout_P2ManualP2",

	"Layout_FileInfoCompIcon",
	"Layout_FileInfoLuigi",
	"Layout_FileInfoMario",
	"Layout_FileInfoSPiece",
	"Layout_FileInfoStar",

	"Layout_GalaxyDetailCoin",
	"Layout_GalaxyDetailComplete",
	"Layout_GalaxyInfoComplete",

	"Layout_GalaxySelectStar",

	"Layout_HipDropGuidanceTxtA",
	"Layout_HipDropGuidanceTxtZ",

	"Layout_MapDomeIconCrown",
	"Layout_MapGalaxyIconCrown",
	"Layout_MapGrandGalaxyStar",

	"Layout_PauseMenuCoin",
	"Layout_PauseMenuStar",

	"Layout_ResultWindowCoin",
	"Layout_ResultWindowPiece",
	"Layout_ResultWindowStar",
	"Layout_ResultWindowTPiece",

	"Layout_ScenarioSelectCoin",

	"Common_Kinopio000",
	"PlayerLeft"
};