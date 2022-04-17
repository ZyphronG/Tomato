#include <iostream>
#include <cstring>
#include "../include/MESGbmg1.h"

namespace MESGbmg1 {

	// Base util
	const char* getElementFromTable(const char** pTable, u32 pNumOfElements, u32 pIndex) {
		if (pIndex < pNumOfElements)
			return pTable[pIndex];
		else
			return 0;
	}

	u32 getElementIndexFromTable(const char** pTable, u32 pNumOfElements, const char* pElementToSearchFor) {
		for (u32 i = 0; i < pNumOfElements; i++) {
			if (strcmp(pTable[i], pElementToSearchFor) == 0)
				return i;
		}

		return atoi(pElementToSearchFor);
	}

	// Misc

	const char* getIconName(u32 i) {
		return getElementFromTable(IconNameTable, sizeof(IconNameTable)/sizeof(char*), i);
	}

	u32 getIconNameId(const char* pIconName) {
		return getElementIndexFromTable(IconNameTable, sizeof(IconNameTable)/sizeof(char*), pIconName);
	}

	const char* getColorName(u32 i) {
		return getElementFromTable(FontColorNames, sizeof(FontColorNames)/sizeof(char*), i);
	}

	u32 getColorNameId(const char* pFontColorName) {
		return getElementIndexFromTable(FontColorNames, sizeof(FontColorNames)/sizeof(char*), pFontColorName);
	}

	const char* getSoundIDName(u8 i) {
		return getElementFromTable(SoundIDs, sizeof(SoundIDs)/sizeof(char*), i);
	}

	u32 getSoundNameIDID(const char* pSoundIDName) {
		return getElementIndexFromTable(SoundIDs, sizeof(SoundIDs)/sizeof(char*), pSoundIDName);
	}

	const char* getMessageTypeName(u8 i) {
		if (i == 0) return "talk";
		else if (i == 1) return "shout";
		else if (i == 2) return "auto";
		else if (i == 4) return "flow";
		else return 0;
	}

	u32 getMessageTypeNameID(const char* pMessageTypeName) {
		if (strcmp(pMessageTypeName, "talk") == 0) return 0;
		else if (strcmp(pMessageTypeName, "shout") == 0) return 1;
		else if (strcmp(pMessageTypeName, "auto") == 0) return 2;
		else if (strcmp(pMessageTypeName, "flow") == 0) return 4;
		else return atoi(pMessageTypeName);
	}

	const char* getCamTypeName(u8 i) {
		if (i == 0) return "normal";
		else if (i == 1) return "cameraID";
		else if (i == 2) return "noCam";
		else return 0;
	}

	u32 getCamTypeNameID(const char* pCamTypeName) {
		if (strcmp(pCamTypeName, "normal") == 0) return 0;
		else if (strcmp(pCamTypeName, "cameraID") == 0) return 1;
		else if (strcmp(pCamTypeName, "noCam") == 0) return 2;
		else return atoi(pCamTypeName);
	}

	const char* getTextBoxTypeName(u8 i) {
		if (i == 0) return "normal";
		else if (i == 4) return "signboard";
		else return 0;
	}

	u32 getTextBoxTypeNameID(const char* pTextBoxTypeName) {
		if (strcmp(pTextBoxTypeName, "normal") == 0) return 0;
		else if (strcmp(pTextBoxTypeName, "signboard") == 0) return 4;
		else return atoi(pTextBoxTypeName);
	}




	const char* getConditionNodeType(u32 i) {
		return getElementFromTable(NodeConditionTypeList, sizeof(NodeConditionTypeList)/sizeof(char*), i);
	}

	u32 getConditionNodeTypeID(const char* pNodeConditionType) {
		return getElementIndexFromTable(NodeConditionTypeList, sizeof(NodeConditionTypeList)/sizeof(char*), pNodeConditionType);
	}

	const char* getConditionMultipleChoice(u32 i) {
		return getElementFromTable(NodeConditionMultipleChoices, sizeof(NodeConditionMultipleChoices)/sizeof(char*), i);
	}

    u32 getConditionMultipleChoiceID(const char* pChoiceName) {
		return getElementIndexFromTable(NodeConditionMultipleChoices, sizeof(NodeConditionMultipleChoices)/sizeof(char*), pChoiceName);
	}

	const char* getEventType(u32 i) {
		return getElementFromTable(NodeEventTypes, sizeof(NodeEventTypes)/sizeof(char*), i);
	}

    u32 getEventTypeID(const char* pEventType) {
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
		u16 EntryNumTmp = EntryNum;

		// convert from big endian to little endian
		SectionBase::swapEndian();
		EntryNum = _byteswap_ushort(EntryNum);
		EntrySize = _byteswap_ushort(EntrySize);

		// this is used to check if the section is using the right endian
		// if so then use the newly swapped endian since the initialized EntryNumTmp
		// is using the wrong endian which makes the number "inverted"
		if (Magic == 'INF1')
			EntryNumTmp = EntryNum;
			
		for (s32 i = 0; i < EntryNumTmp; i++) {
			INF1Entry* InfEnt = getInfoData(i);
			InfEnt->TextAddress = _byteswap_ulong(InfEnt->TextAddress);
			InfEnt->CameraId = _byteswap_ushort(InfEnt->CameraId);
		}
	}

	INF1Entry* INF1::getInfoData(u32 IndexID) {
		return &((INF1Entry*)((char*)this + sizeof(INF1)))[IndexID];
	}

	void INF1::setUp(u32 pEntryNum) {
		Magic = 'INF1';
		SectionSize = 0x10 + pEntryNum * 0xC;
		EntryNum = pEntryNum;
		EntrySize = 0xC;
		addPadding();
	}



	void DAT1::swapEndian() {
		u32 SectionSizeTmp = SectionSize;

		SectionBase::swapEndian();

		if (Magic == 'DAT1')
			SectionSizeTmp = SectionSize;

		// swap bytes of data
		c16* MessageData = getTextData(0);
		SectionSizeTmp = (SectionSizeTmp - sizeof(DAT1)) / 2;

		for (u32 i = 0; i < SectionSizeTmp; i++) {
			MessageData[i] = _byteswap_ushort(MessageData[i]);

			// event tag
			if ( (Magic == 'DAT1' && MessageData[i] == 0x1A) || (Magic != 'DAT1' && MessageData[i] == 0x1A00) ) {
				i++;
				u8 EventSize = *((u8*)&MessageData[i]);
				u8 EventType = *((u8*)&MessageData[i] + 1);
				i++;

				if (EventType == 2 || EventType == 5 || EventType == 0xFF) {
					MessageData[i] = _byteswap_ushort(MessageData[i]);	// swap EventTypeType
					i += (EventSize/2) - 3;		// skip the rest of the event
				}
				else if (EventType == 6 || EventType == 7) {
					MessageData[i] = _byteswap_ushort(MessageData[i]);		// swap EventTypeType
					u32* C = (u32*)(&MessageData[i + 1]);
					*C = _byteswap_ulong(*C);
					C++;
					*C = _byteswap_ulong(*C);
					i += (EventSize/2) - 3;		// skip the rest of the event
				}
				else {
					// swap endian
					for (u32 f = 0; f < EventSize / 2 - 2; f++) {
						MessageData[i] = _byteswap_ushort(MessageData[i]);
						i++;
					}
					i--;
				}
			}
		}
	}

	c16* DAT1::getTextData(u32 Offset) {
		return (c16*)((char*)this + sizeof(DAT1) + Offset);
	}

	void DAT1::setUp() {
		Magic = 'DAT1';
		SectionSize = 0x8;
	}



	void FLW1::swapEndian() {
		u16 NodeNumTmp = NodeNum;
		u16 BranchNodeNumTmp = BranchNodeNum;

		SectionBase::swapEndian();
		NodeNum = _byteswap_ushort(NodeNum);
		BranchNodeNum = _byteswap_ushort(BranchNodeNum);

		if (Magic == 'FLW1') {
			NodeNumTmp = NodeNum;
			BranchNodeNumTmp = BranchNodeNum;
		}

		for (s32 i = 0; i < NodeNumTmp; i++) {
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

		u16* BranchNode = (u16*)getNodeEntry(NodeNumTmp);
		for (s32 i = 0; i < BranchNodeNumTmp; i++) {
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

	NodeEntryText* FLW1::getNodeEntry(u32 Index) {
		return &((NodeEntryText*)((char*)this + sizeof(FLW1)))[Index];
	}

	u16* FLW1::getBranchNode(u32 Index) {
		return &((u16*)(getNodeEntry(NodeNum)))[Index];
	}

	char* FLW1::getBranchNodeValid(u32 Index) {
		return &((char*)(getBranchNode(BranchNodeNum)))[Index];
	}

	void FLW1::createBranchValiditySection() {
		u16* Branch = getBranchNode(0);
		char* Valid = getBranchNodeValid(0);

		// add section size
		SectionSize += BranchNodeNum;

		for (u32 i = 0; i < BranchNodeNum; i++) {
			if (Branch[i] == 0xFFFF)
				Valid[i] = -1;
			else
				Valid[i] = 0;
		}
	}
}