#include <iostream>
#include <cstring>
#include "MESGbmg1.h"

namespace MESGbmg1 {

	// Base util
	const char* getElementFromTable(const char** pTable, unsigned long pNumOfElements, unsigned long pIndex) {
		if (pIndex < pNumOfElements)
			return pTable[pIndex];
		else
			return 0;
	}

	unsigned long getElementIndexFromTable(const char** pTable, unsigned long pNumOfElements, const char* pElementToSearchFor) {
		for (unsigned long i = 0; i < pNumOfElements; i++) {
			if (strcmp(pTable[i], pElementToSearchFor) == 0)
				return i;
		}

		return atoi(pElementToSearchFor);
	}

	// Misc

	const char* getIconName(unsigned long i) {
		return getElementFromTable(IconNameTable, sizeof(IconNameTable)/sizeof(char*), i);
	}

	unsigned long getIconNameId(const char* pIconName) {
		return getElementIndexFromTable(IconNameTable, sizeof(IconNameTable)/sizeof(char*), pIconName);
	}

	const char* getColorName(unsigned long i) {
		if (i % 0x100 != 0)
			return 0;
		
		return getElementFromTable(FontColorNames, sizeof(FontColorNames)/sizeof(char*), i/0x100);
	}

	unsigned long getColorNameId(const char* pFontColorName) {
		unsigned long i = 0;
		while (true) {
			const char* FontColorName = getColorName(i*0x100);

			if (!FontColorName)
				return atoi(pFontColorName);

			if (strcmp(pFontColorName, FontColorName) == 0)
				return i * 0x100;

			i++;
		}
	}

	const char* getSoundIDName(unsigned char i) {
		return getElementFromTable(SoundIDs, sizeof(SoundIDs)/sizeof(char*), i);
	}

	unsigned long getSoundNameIDID(const char* pSoundIDName) {
		return getElementIndexFromTable(SoundIDs, sizeof(SoundIDs)/sizeof(char*), pSoundIDName);
	}

	const char* getMessageTypeName(unsigned char i) {
		if (i == 0) return "talk";
		else if (i == 1) return "shout";
		else if (i == 2) return "auto";
		else if (i == 4) return "flow";
		else return 0;
	}

	unsigned long getMessageTypeNameID(const char* pMessageTypeName) {
		if (strcmp(pMessageTypeName, "talk") == 0) return 0;
		else if (strcmp(pMessageTypeName, "shout") == 0) return 1;
		else if (strcmp(pMessageTypeName, "auto") == 0) return 2;
		else if (strcmp(pMessageTypeName, "flow") == 0) return 4;
		else return atoi(pMessageTypeName);
	}

	const char* getCamTypeName(unsigned char i) {
		if (i == 0) return "normal";
		else if (i == 1) return "cameraID";
		else if (i == 2) return "noCam";
		else return 0;
	}

	unsigned long getCamTypeNameID(const char* pCamTypeName) {
		if (strcmp(pCamTypeName, "normal") == 0) return 0;
		else if (strcmp(pCamTypeName, "cameraID") == 0) return 1;
		else if (strcmp(pCamTypeName, "noCam") == 0) return 2;
		else return atoi(pCamTypeName);
	}

	const char* getTextBoxTypeName(unsigned char i) {
		if (i == 0) return "normal";
		else if (i == 4) return "signboard";
		else return 0;
	}

	unsigned long getTextBoxTypeNameID(const char* pTextBoxTypeName) {
		if (strcmp(pTextBoxTypeName, "normal") == 0) return 0;
		else if (strcmp(pTextBoxTypeName, "signboard") == 0) return 4;
		else return atoi(pTextBoxTypeName);
	}




	const char* getConditionNodeType(unsigned long i) {
		return getElementFromTable(NodeConditionTypeList, sizeof(NodeConditionTypeList)/sizeof(char*), i);
	}

	unsigned long getConditionNodeTypeID(const char* pNodeConditionType) {
		return getElementIndexFromTable(NodeConditionTypeList, sizeof(NodeConditionTypeList)/sizeof(char*), pNodeConditionType);
	}

	const char* getConditionMultipleChoice(unsigned long i) {
		return getElementFromTable(NodeConditionMultipleChoices, sizeof(NodeConditionMultipleChoices)/sizeof(char*), i);
	}

    unsigned long getConditionMultipleChoiceID(const char* pChoiceName) {
		return getElementIndexFromTable(NodeConditionMultipleChoices, sizeof(NodeConditionMultipleChoices)/sizeof(char*), pChoiceName);
	}

	const char* getEventType(unsigned long i) {
		return getElementFromTable(NodeEventTypes, sizeof(NodeEventTypes)/sizeof(char*), i);
	}

    unsigned long getEventTypeID(const char* pEventType) {
		return getElementIndexFromTable(NodeEventTypes, sizeof(NodeEventTypes)/sizeof(char*), pEventType);
	}




	void FileHeader::swapEndian() {
		// convert from big endian to little endian
		FileHeaderMagic = _byteswap_ulong(FileHeaderMagic);
		SectionMagic = _byteswap_ulong(SectionMagic);
		FLW1SectionAdr = _byteswap_ulong(FLW1SectionAdr);
		SectionNum = _byteswap_ulong(SectionNum);
	}

	void FileHeader::setUp() {
		FileHeaderMagic = 'MESG';
		SectionMagic = 'bmg1';
		SectionNum = 3;
		unk = 2;
	}


	void SectionBase::swapEndian() {
		Magic = _byteswap_ulong(Magic);
		SectionSize = _byteswap_ulong(SectionSize);
	}

	void SectionBase::addPadding() {
		while (SectionSize % 0x20 != 0)
			SectionSize++;
	}


	void INF1::swapEndian() {
		unsigned short EntryNumTmp = EntryNum;

		// convert from big endian to little endian
		SectionBase::swapEndian();
		EntryNum = _byteswap_ushort(EntryNum);
		EntrySize = _byteswap_ushort(EntrySize);

		// this is used to check if the section is using the right endian
		// if so then use the newly swapped endian since the initialized EntryNumTmp
		// is using the wrong endian which makes the number "inverted"
		if (Magic == 'INF1')
			EntryNumTmp = EntryNum;
			
		for (long i = 0; i < EntryNumTmp; i++) {
			INF1Entry* InfEnt = getInfoData(i);
			InfEnt->TextAddress = _byteswap_ulong(InfEnt->TextAddress);
			InfEnt->CameraId = _byteswap_ushort(InfEnt->CameraId);
		}
	}

	INF1Entry* INF1::getInfoData(unsigned long IndexID) {
		return &((INF1Entry*)((char*)this + sizeof(INF1)))[IndexID];
	}

	void INF1::setUp(unsigned long pEntryNum) {
		Magic = 'INF1';
		SectionSize = 0x10 + pEntryNum * 0xC;
		EntryNum = pEntryNum;
		EntrySize = 0xC;
		addPadding();
	}



	void DAT1::swapEndian() {
		unsigned long SectionSizeTmp = SectionSize;

		SectionBase::swapEndian();

		if (Magic == 'DAT1')
			SectionSizeTmp = SectionSize;

		// swap bytes of data
		wchar_t* MessageData = getTextData(0);
		SectionSizeTmp = (SectionSizeTmp - sizeof(DAT1)) / 2;

		for (unsigned long i = 0; i < SectionSizeTmp; i++)
			MessageData[i] = _byteswap_ushort(MessageData[i]);
	}

	wchar_t* DAT1::getTextData(unsigned long Offset) {
		return (wchar_t*)((char*)this + sizeof(DAT1) + Offset);
	}

	void DAT1::setUp() {
		Magic = 'DAT1';
		SectionSize = 0x8;
	}



	void FLW1::swapEndian() {
		unsigned short NodeNumTmp = NodeNum;
		unsigned short BranchNodeNumTmp = BranchNodeNum;

		SectionBase::swapEndian();
		NodeNum = _byteswap_ushort(NodeNum);
		BranchNodeNum = _byteswap_ushort(BranchNodeNum);

		if (Magic == 'FLW1') {
			NodeNumTmp = NodeNum;
			BranchNodeNumTmp = BranchNodeNum;
		}

		for (long i = 0; i < NodeNumTmp; i++) {
			NodeEntryText* NodeEntryTxt = getNodeEntry(i);
			
			if (NodeEntryTxt->FlowType == 1) {
				NodeEntryTxt->TextId = _byteswap_ushort(NodeEntryTxt->TextId);
				NodeEntryTxt->NextFlowId = _byteswap_ushort(NodeEntryTxt->NextFlowId);
			}
			else if (NodeEntryTxt->FlowType == 2) {
				NodeEntryCondition* NodeEntryCon = (NodeEntryCondition*)NodeEntryTxt;
				NodeEntryCon->ConditionType = _byteswap_ushort(NodeEntryCon->ConditionType);
				NodeEntryCon->Arg = _byteswap_ushort(NodeEntryCon->Arg);
				NodeEntryCon->BranchNodeId = _byteswap_ushort(NodeEntryCon->BranchNodeId);
			}
			else {
				NodeEntryEvent* NodeEntryEve = (NodeEntryEvent*)NodeEntryTxt;
				NodeEntryEve->BranchNodeId = _byteswap_ushort(NodeEntryEve->BranchNodeId);
				NodeEntryEve->Arg = _byteswap_ulong(NodeEntryEve->Arg);
			}
		}

		unsigned short* BranchNode = (unsigned short*)getNodeEntry(NodeNumTmp);
		for (long i = 0; i < BranchNodeNumTmp; i++) {
			BranchNode[i] = _byteswap_ushort(BranchNode[i]);
		}

	}

	void FLW1::setUp() {
		Magic = 'FLW1';
		SectionSize = 0x10;
		NodeNum = 0;
		BranchNodeNum = 0;
		memset(&Padding, 0, 4);
	}

	NodeEntryText* FLW1::getNodeEntry(unsigned long Index) {
		return &((NodeEntryText*)((char*)this + sizeof(FLW1)))[Index];
	}

	unsigned short* FLW1::getBranchNode(unsigned long Index) {
		return &((unsigned short*)(getNodeEntry(NodeNum)))[Index];
	}

	char* FLW1::getBranchNodeValid(unsigned long Index) {
		return &((char*)(getBranchNode(BranchNodeNum)))[Index];
	}

	void FLW1::createBranchValiditySection() {
		unsigned short* Branch = getBranchNode(0);
		char* Valid = getBranchNodeValid(0);

		// add section size
		SectionSize += BranchNodeNum;

		for (unsigned long i = 0; i < BranchNodeNum; i++) {
			if (Branch[i] == 0xFFFF)
				Valid[i] = -1;
			else
				Valid[i] = 0;
		}
	}
}