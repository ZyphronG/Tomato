#if !defined(_WIN32) && !defined(_WIN64)
#include <byteswap.h>
#define _byteswap_ushort bswap_16
#define _byteswap_ulong bswap_32
#endif

#include <fstream>
#include <iostream>
#include <cstring>
#include "Tomato.h"
#include "MESGbmg1.cpp"
#include "Bcsv.cpp"
#include "XML.cpp"
#include "FileHolder.cpp"



BmgFileHolder::BmgFileHolder(const char* pFileName) {
	mFileHolder = FileHolder(pFileName);
	Header = 0;
	InfoSection = 0;
	DataSection = 0;
	FlowSection = 0;
	isSwapEndian = false;
	isBmgBigEndian = false;
}

bool BmgFileHolder::loadBmg() {
	if (mFileHolder.loadFile()) {
		std::cout << "The BMG's file size is " << mFileHolder.mFileSize << ".\n";

		// check endian
		unsigned long HeaderName = ((MESGbmg1::FileHeader*)mFileHolder.mFileData)->FileHeaderMagic;
		if (HeaderName == 'GSEM')
			isSwapEndian = true;
		else if (HeaderName != 'MESG') {
			std::cout << "The input file is not a valid Super Mario Galaxy BMG file.\n";
			return false;
		}

		// check BMG endian
		if (strncmp(mFileHolder.mFileData, "MESG", 4) == 0)
			isBmgBigEndian = true;



		

		Header = (MESGbmg1::FileHeader*)mFileHolder.mFileData;
		if (isSwapEndian)
			Header->swapEndian();

		// check if no sections exist
		if (Header->SectionNum == 0) {
			std::cout << "There are no Sections in the given BMG.\n";
			return false;
		}
		else if (Header->SectionNum > 4)
			std::cout << "WARNING: There are more than the usual 4 Sections in the given BMG.\n";

		// set up sections from Raw Bmg
		unsigned long MagicTmp;
		MESGbmg1::SectionBase* SecBase = (MESGbmg1::SectionBase*)(mFileHolder.mFileData + sizeof(MESGbmg1::FileHeader));

		for (unsigned long i = 0; i < Header->SectionNum; i++) {
			// load the Magic considering the Endian
			if (isSwapEndian)
				MagicTmp = _byteswap_ulong(SecBase->Magic);
			else
				MagicTmp = SecBase->Magic;

			switch (MagicTmp) {
			case 'INF1':
				InfoSection = (MESGbmg1::INF1*)SecBase;
				if (isSwapEndian)
					InfoSection->swapEndian();
				break;
			case 'DAT1':
				DataSection = (MESGbmg1::DAT1*)SecBase;
				if (isSwapEndian)
					DataSection->swapEndian();
				break;
			case 'FLW1':
				FlowSection = (MESGbmg1::FLW1*)SecBase;
				if (isSwapEndian)
					FlowSection->swapEndian();
				break;
			case 'FLI1':
				// this section is not supported so just skip
				if (isSwapEndian)
					SecBase->swapEndian();
				break;

			default:
				std::cout << "An unsupported Section was found while parsing the BMG or this file is corrupted.\n";
				return false;
			}
			SecBase = (MESGbmg1::SectionBase*)((char*)SecBase + SecBase->SectionSize);
		}
			

		return true;
	}
	else {
		std::cout << mFileHolder.mFileData << " isn't exist.\n";	// File isn't exist. the funny
		return false;
	}
}













// XML Write Tools

XmlWriter::XmlWriter(const char* pXmlFileName, const BmgFileHolder* pBMG, const BCSV* pBCSV) {
	FileName = pXmlFileName;
	BMG = pBMG;
	bcsv = pBCSV;
	ConvertedFlowStorage = 0;
}

bool XmlWriter::writeXml() {
	char XMLEntry[256];
	char MessageTypeName[64];
	char MessageTextBoxTypeName[64];
	char MessageSoundName[64];
	char MessageCamTypeName[64];
	ConvertedFlowStorage = new char[BMG->FlowSection->NodeNum];
	memset(ConvertedFlowStorage, 0, BMG->FlowSection->NodeNum);

	outFile.open(FileName, std::ios::out);

	for (unsigned long i = 0; i < BMG->InfoSection->EntryNum; i++) {
		MESGbmg1::INF1Entry* InfEnt = BMG->InfoSection->getInfoData(i);
		const char* MessageID = getStringFromBcsv(i);

		std::cout << "\rConverting Entry " << i + 1 << " / " << BMG->InfoSection->EntryNum << "..." << std::flush;

		if (MessageID) {
			const char* StoreChar = MESGbmg1::getMessageTypeName(InfEnt->MessageType);
			if (StoreChar) strcpy(MessageTypeName, StoreChar);
			else sprintf(MessageTypeName, "%d", InfEnt->MessageType);

			StoreChar = MESGbmg1::getTextBoxTypeName(InfEnt->MessageBoxType);
			if (StoreChar) strcpy(MessageTextBoxTypeName, StoreChar);
			else sprintf(MessageTextBoxTypeName, "%d", InfEnt->MessageBoxType);

			StoreChar = MESGbmg1::getSoundIDName(InfEnt->SoundId);
			if (StoreChar) strcpy(MessageSoundName, StoreChar);
			else sprintf(MessageSoundName, "%d", InfEnt->SoundId);

			StoreChar = MESGbmg1::getCamTypeName(InfEnt->CamType);
			if (StoreChar) strcpy(MessageCamTypeName, StoreChar);
			else sprintf(MessageCamTypeName, "%d", InfEnt->CamType);

			sprintf(XMLEntry,
			"<message label=\"%s\">\n\t<info messageType=\"%s\" textbox=\"%s\" sound=\"%s\" camType=\"%s\" cameraID=\"%d\" messageArea=\"%d\" alreadyTalked=\"%d\"/>\n",
			MessageID, MessageTypeName, MessageTextBoxTypeName, MessageSoundName, MessageCamTypeName, InfEnt->CameraId, InfEnt->MessageAreaId, InfEnt->Unknown);

			outFile << XMLEntry;

			// write message string if exist
			if (*BMG->DataSection->getTextData(InfEnt->TextAddress) != 0) {
				outFile << "\t<string>\n\t\t";
				createAndWriteMessage(i);
				outFile << "\n\t</string>\n";
			}

			// write flow container if exist
			if (isNeedWriteFlowSection(i)) {
				outFile << "\t<flow>\n";
				findNodeByMessageID(i);
				outFile << "\t</flow>\n";
			}

			outFile << "</message>\n";
		}
		else {
			std::cout << "\nMessage entry was not found in the MessageID.tbl! Skipping...\n";
		}
	}

	// write non-assigned flows / leftovers
	for (unsigned long i = 0; i < BMG->FlowSection->NodeNum; i++) {
		if (ConvertedFlowStorage[i] == 0)
			writeFlows(i, false);
	}

	outFile.close();
	delete[] ConvertedFlowStorage;
	ConvertedFlowStorage = 0;

	return true;
}

const char* XmlWriter::getStringFromBcsv(unsigned long Index) {
	unsigned long bcsvIndex = bcsv->findElement("Index", Index);
	const char* StringOut;

	if (bcsvIndex < bcsv->mEntryNum && bcsv->getValue(bcsvIndex, "MessageId", &StringOut))
		return StringOut;
	else
		return 0;
}

void XmlWriter::createAndWriteMessage(unsigned long Index) {
	unsigned long TextOffset = BMG->InfoSection->getInfoData(Index)->TextAddress;
	unsigned char character;
	
	while (true) {
		character = *BMG->DataSection->getTextData(TextOffset);

		if (character == 0)
			break;

		// is new line
		if (character == 0xA) {
			outFile << "\n\t\t";
			TextOffset +=2;
		}
		// is Event
		else if (character == 0x1A) {
			TextOffset = createEventText(TextOffset);
		}
		// is note
		else if (*BMG->DataSection->getTextData(TextOffset) == 0x266A) {
			outFile << "<note/>";
			TextOffset += 2;
		}
		else {
			outFile << character;
			TextOffset += 2;
		}
	}
}

unsigned long XmlWriter::createEventText(unsigned long TextOffset) {
	unsigned char EntrySize = *((unsigned char*)BMG->DataSection->getTextData(TextOffset + 2));
	unsigned char EntryType = *((unsigned char*)BMG->DataSection->getTextData(TextOffset + 2) + 1);
	unsigned long EntryTypeType = *BMG->DataSection->getTextData(TextOffset + 4);
	
	if (EntryType == 1) {
		switch (EntryTypeType) {
		case 0:
			outFile << "<wait time=\"" << *BMG->DataSection->getTextData(TextOffset + 6) << "\"/>";
			break;
		case 1:
			outFile << "<newpage/>";
			break;
		// case 2 unknown
		case 3:
			outFile << "<centerText/>";
			break;
		default:
			goto TemporarySolution;
		}
	}
	else if (EntryType == 2) {
		if (EntryTypeType == 0) {
			outFile << "<sound src=\"";
			unsigned char counter = 6;
			while (counter < EntrySize) {
				outFile << *((unsigned char*)BMG->DataSection->getTextData(TextOffset + counter) + 1);
				counter += 2;
			}
			outFile << "\"/>";
		}
		else
			goto TemporarySolution;
	}
	else if (EntryType == 3) {
		const char* IconName = MESGbmg1::getIconName(EntryTypeType);
		if (IconName)
			outFile << "<icon src=\"" << IconName << "\"/>";
		else
			outFile << "<icon src=\"" << EntryTypeType << "\"/>";
	}
	else if (EntryType == 4) {
		outFile << "<font size=\"";
		if (EntryTypeType == 0) outFile << "small";
		else if (EntryTypeType == 1) outFile << "normal";
		else if (EntryTypeType == 2) outFile << "large";
		else outFile << EntryTypeType;
		outFile << "\"/>";
	}
	else if (EntryType == 5) {
		if (EntryTypeType == 0) {
			outFile << "<player name=\"";
			unsigned char NameType = *(unsigned char*)BMG->DataSection->getTextData(TextOffset + 6);
			if (NameType == 0) outFile << "normal";
			else if (NameType == 1) outFile << "formal";
			else if (NameType == 2) outFile << "moustache";
			else outFile << NameType;
			outFile << "\"/>";
		}
		else goto TemporarySolution;
	}
	else if (EntryType == 6) {
		outFile << "<valInt arg1=\"" << EntryTypeType << "\" arg2=\"" << *(long*)BMG->DataSection->getTextData(TextOffset + 6) << "\" arg3=\"" << *(long*)BMG->DataSection->getTextData(TextOffset + 10) << "\"/>";
	}
	else if (EntryType == 7) {
		outFile << "<valStr arg1=\"" << EntryTypeType << "\" arg2=\"" << *(long*)BMG->DataSection->getTextData(TextOffset + 6) << "\" arg3=\"" << *(long*)BMG->DataSection->getTextData(TextOffset + 10) << "\"/>";
	}
	else if (EntryType == 255) {
		if (EntryTypeType == 0) {
			const char* FontColorName = MESGbmg1::getColorName(*(unsigned char*)BMG->DataSection->getTextData(TextOffset + 6));
			outFile << "<font color=\"";
			if (FontColorName) outFile << FontColorName;
			else outFile << (unsigned short)*(unsigned char*)BMG->DataSection->getTextData(TextOffset + 6);
			outFile << "\"/>";
		}
		else goto TemporarySolution;
	}
	else {
		TemporarySolution:
		char letter = 'a';
		unsigned char counter = 4;
		char text[64];
		// default event text
		sprintf(text, "<e type=\"%d\"", EntryType);
		outFile << text;
		while (counter < EntrySize) {
			sprintf(text, " %c=\"%d\"", letter, *(unsigned short*)BMG->DataSection->getTextData(TextOffset + counter));
			outFile << text;
			letter++;
			counter += 2;
		}
		outFile << "/>";
	}

	return TextOffset + EntrySize;
}

// this function checks if the message ID text node is referenced by any other node.
// if not then create flow container
bool XmlWriter::isNeedWriteFlowSection(unsigned long pMessageId) {
	for (unsigned long i = 0; i < BMG->FlowSection->NodeNum; i++) {
		MESGbmg1::NodeEntryText* TextNode = BMG->FlowSection->getNodeEntry(i);

		if (TextNode->FlowType == 1 && TextNode->TextId == pMessageId) {
			// check for reference of the found node entry in other nodes
			for (unsigned long c = 0; c < BMG->FlowSection->NodeNum; c++) {
				if (c != i) {
					MESGbmg1::NodeEntryText* TextNodeSub = BMG->FlowSection->getNodeEntry(c);

					if (TextNodeSub->FlowType == 1 && TextNodeSub->NextFlowId == i)
						goto CheckForNextNode;

					if (TextNodeSub->FlowType == 2) {
						MESGbmg1::NodeEntryCondition* ConNodeSub = (MESGbmg1::NodeEntryCondition*)TextNodeSub;
						if (*BMG->FlowSection->getBranchNode(ConNodeSub->BranchNodeId) == i || *BMG->FlowSection->getBranchNode(ConNodeSub->BranchNodeId + 1) == i)
							goto CheckForNextNode;
					}

					if (TextNodeSub->FlowType == 3) {
						MESGbmg1::NodeEntryEvent* EventNodeSub = (MESGbmg1::NodeEntryEvent*)TextNodeSub;
						if (*BMG->FlowSection->getBranchNode(EventNodeSub->BranchNodeId) == i)
							goto CheckForNextNode;
					}
				}
			}

			return true;
		}

		CheckForNextNode:
		continue;
	}

	return false;
}

void XmlWriter::findNodeByMessageID(unsigned long pMessageId) {
	for (unsigned long i = 0; i < BMG->FlowSection->NodeNum; i++) {
		MESGbmg1::NodeEntryText* TextNode = BMG->FlowSection->getNodeEntry(i);

		if (TextNode->FlowType == 1 && TextNode->TextId == pMessageId)
			writeFlows(i, true);
	}
}

void XmlWriter::writeFlows(unsigned long pNodeID, bool pIsInContainer) {
	const char* TemporaryCharStorage;

	if (pNodeID == 0xFFFF || ConvertedFlowStorage[pNodeID] == 1)
		return;

	ConvertedFlowStorage[pNodeID] = 1;
	MESGbmg1::NodeEntryText* TextNode = BMG->FlowSection->getNodeEntry(pNodeID);
	if (pIsInContainer)
		outFile << "\t\t";
	outFile << "<node name=\"Node_" << pNodeID << "\" type=\"";

	if (TextNode->FlowType == 1) {
		outFile << "text\" messageID=\"" << getStringFromBcsv(TextNode->TextId) << "\" nextNode=\"";
		// write next flow if is exist
		if (TextNode->NextFlowId == 0xFFFF)
			outFile << "none\"/>\n";
		else {
			outFile << "Node_" << TextNode->NextFlowId << "\"/>\n";
			writeFlows(TextNode->NextFlowId, pIsInContainer);
		}
	}
	else if (TextNode->FlowType == 2) {
		MESGbmg1::NodeEntryCondition* ConNode = (MESGbmg1::NodeEntryCondition*)TextNode;
		TemporaryCharStorage = MESGbmg1::getConditionNodeType(ConNode->ConditionType);
		outFile << "condition\" condition=\"";

		if (TemporaryCharStorage) outFile << TemporaryCharStorage;
		else outFile << ConNode->ConditionType;

		if (ConNode->ConditionType == 0) {
			TemporaryCharStorage = MESGbmg1::getConditionMultipleChoice(ConNode->Arg);
			outFile << "\" choice=\"";
			if (TemporaryCharStorage) outFile << TemporaryCharStorage;
			else outFile << ConNode->Arg;
		}
		else
			outFile << "\" arg=\"" << ConNode->Arg;

		outFile << "\" trueNode=\"";

		unsigned short TrueFlow = *BMG->FlowSection->getBranchNode(ConNode->BranchNodeId);

		if (TrueFlow == 0xFFFF)
			outFile << "none";
		else
			outFile << "Node_" << TrueFlow;

		outFile << "\" falseNode=\"";
		unsigned short FalseFlow = *BMG->FlowSection->getBranchNode(ConNode->BranchNodeId + 1);

		if (FalseFlow == 0xFFFF)
			outFile << "none";
		else
			outFile << "Node_" << FalseFlow;

		outFile << "\"/>\n";
		writeFlows(TrueFlow, pIsInContainer);
		writeFlows(FalseFlow, pIsInContainer);
	}
	else if (TextNode->FlowType == 3) {
		MESGbmg1::NodeEntryEvent* EventNode = (MESGbmg1::NodeEntryEvent*)TextNode;
		TemporaryCharStorage = MESGbmg1::getEventType(EventNode->EventType);
		outFile << "event\" event=\"";

		if (TemporaryCharStorage) outFile << TemporaryCharStorage;
		else outFile << (short)EventNode->EventType;

		outFile << "\" arg=\"" << EventNode->Arg;

		outFile << "\" nextNode=\"";

		unsigned short NextFlow = *BMG->FlowSection->getBranchNode(EventNode->BranchNodeId);

		if (NextFlow == 0xFFFF)
			outFile << "none";
		else
			outFile << "Node_" << NextFlow;

		outFile << "\"/>\n";
		writeFlows(NextFlow, pIsInContainer);
	}
	else
		outFile << "unk\"/>\n";
}







// BMG write Tools

BmgWriter::BmgWriter(const char* pXmlFileName, const char* pBmgFileName, const char* pTblFileName) {
	BMG = new BmgFileHolder(pBmgFileName);
	bcsv = 0;
	XML = new FileHolder(pXmlFileName);
	mTblFileName = pTblFileName;
}

bool BmgWriter::writeBmg(bool pIsLe) {
	// check for number of entries
	if (!XML->loadFile()) {
		std::cout << XML->mFileName << " could not be opened.\n";
		return false;
	}
	
	unsigned long MessageEntryNum = XmlUtil::getNumberOfEntries("message", XML);

	// create BCSV
	bcsv = BCSV::createNewBcsv(MessageEntryNum);
	bcsv->addField("MessageId", BCSV::StringOffset);
	bcsv->addField("Index", BCSV::Short);

	// start BMG stuff
	BMG->mFileHolder.mFileData = new char[0x100000];
	// clear space
	memset(BMG->mFileHolder.mFileData, 0, 0x100000);
	
	// set up header
	BMG->Header = (MESGbmg1::FileHeader*)BMG->mFileHolder.mFileData;
	BMG->Header->setUp();

	// set up info section
	BMG->InfoSection = (MESGbmg1::INF1*)(BMG->mFileHolder.mFileData + sizeof(MESGbmg1::FileHeader));
	BMG->InfoSection->setUp(MessageEntryNum);

	// set up data section
	BMG->DataSection = (MESGbmg1::DAT1*)(BMG->mFileHolder.mFileData + sizeof(MESGbmg1::FileHeader) + BMG->InfoSection->SectionSize);
	BMG->DataSection->setUp();



	// gather message ID names and create and write messages
	XMLEntry XmlEntry = XMLEntry(XML);
	XMLSubEntry XmlSubEntry = XMLSubEntry(&XmlEntry);
	char MessageLabel[128];
	char MessageValue[64];

	

	for (unsigned long i = 0; i < MessageEntryNum; i++) {
		if (XmlEntry.getNextEntry("message") && XmlEntry.getEntryInfo("label", MessageLabel)) {

			std::cout << "\rConverting Entry " << i + 1 << " / " << MessageEntryNum << "..." << std::flush;

			bcsv->writeEntry(i, "Index", &i);
			bcsv->writeEntry(i, "MessageId", &MessageLabel);


			MESGbmg1::INF1Entry* infoEntry = BMG->InfoSection->getInfoData(i);

			if (XmlSubEntry.findSubEntry("info")) {
				// message type
				if (XmlSubEntry.getEntryInfo("messageType", MessageValue)) infoEntry->MessageType = MESGbmg1::getMessageTypeNameID(MessageValue);
				else std::cout << "\nMessageType info is Missing in " << MessageLabel << ". Using default values...\n";
				// Textbox
				if (XmlSubEntry.getEntryInfo("textbox", MessageValue)) infoEntry->MessageBoxType = MESGbmg1::getTextBoxTypeNameID(MessageValue);
				else std::cout << "\nTextbox info is Missing in " << MessageLabel << ". Using default values...\n";
				// Sound
				if (XmlSubEntry.getEntryInfo("sound", MessageValue)) infoEntry->SoundId = MESGbmg1::getSoundNameIDID(MessageValue);
				else std::cout << "\nSound info is Missing in " << MessageLabel << ". Using default values...\n";
				// CamType
				if (XmlSubEntry.getEntryInfo("camType", MessageValue)) infoEntry->CamType = MESGbmg1::getCamTypeNameID(MessageValue);
				else std::cout << "\nCamType info is Missing in " << MessageLabel << ". Using default values...\n";
				// CameraID
				if (XmlSubEntry.getEntryInfo("cameraID", MessageValue)) infoEntry->CameraId = atoi(MessageValue);
				else std::cout << "\nCameraID info is Missing in " << MessageLabel << ". Using default values...\n";
				// MessageArea
				if (XmlSubEntry.getEntryInfo("messageArea", MessageValue)) infoEntry->MessageAreaId = atoi(MessageValue);
				else {
					infoEntry->MessageAreaId = 255;
					std::cout << "\nMessageArea info is Missing in " << MessageLabel << ". Using default values...\n";
				}
				// unk
				if (XmlSubEntry.getEntryInfo("alreadyTalked", MessageValue)) infoEntry->Unknown = atoi(MessageValue);
				else {
					infoEntry->Unknown = 255;
					std::cout << "\nAlreadyTalked info is Missing in " << MessageLabel << ". Using default values...\n";
				}
			}
			else {
				infoEntry->MessageAreaId = 255;
				infoEntry->Unknown = 255;
				std::cout << "\n" << MessageLabel << " is missing an info section. Using default values...\n";
			}

			// start write message data
			if (XmlSubEntry.findSubEntry("string")) {
				if (XmlSubEntry.mIsLongEntry) {
					BmgTextGenerator MessageCreator = BmgTextGenerator(&XmlSubEntry);
					if (MessageCreator.createMessage())
						infoEntry->TextAddress = tryAddNewTextData(MessageCreator.mMessageData, MessageCreator.mMessagePos);
					else {
						infoEntry->TextAddress = tryAddNewTextData(L"", 1);
						std::cout << "\nWarning: " << MessageLabel << " string data is invalid! Empty text will be used instead.\n";
					}
				}
				else
					infoEntry->TextAddress = tryAddNewTextData(L"", 1);
			}
			else {
				infoEntry->TextAddress = tryAddNewTextData(L"", 1);
				//std::cout << "\nWarning: " << MessageLabel << " has no string data! Empty text will be used instead.\n";
			}
		}
	}

	std::cout << "\n";

	BMG->DataSection->addPadding();

	// set up data section
	BMG->FlowSection = (MESGbmg1::FLW1*)(BMG->mFileHolder.mFileData + sizeof(MESGbmg1::FileHeader) + BMG->InfoSection->SectionSize + BMG->DataSection->SectionSize);
	BMG->FlowSection->setUp();

	unsigned long FlowEntryNum = XmlUtil::getNumberOfEntries("node", XML);
	BMG->FlowSection->NodeNum = FlowEntryNum;
	BMG->FlowSection->SectionSize += FlowEntryNum * 0x8;
	XmlEntry.reset();

	for (unsigned long i = 0; i < FlowEntryNum; i++) {
		std::cout << "\rConverting Flow " << i + 1 << " / " << FlowEntryNum << "..." << std::flush;

		XmlEntry.getNextEntry("node");
		XmlEntry.getEntryInfo("type", MessageValue);

		if (strcmp(MessageValue, "text") == 0) {
			if (!generateTextNode(BMG->FlowSection->getNodeEntry(i), XmlEntry))
				return false;
		}
		else if (strcmp(MessageValue, "condition") == 0) {
			if (!generateConditionNode((MESGbmg1::NodeEntryCondition*)BMG->FlowSection->getNodeEntry(i), XmlEntry))
				return false;
		}
		else if (strcmp(MessageValue, "event") == 0) {
			if (!generateEventNode((MESGbmg1::NodeEntryEvent*)BMG->FlowSection->getNodeEntry(i), XmlEntry))
				return false;
		}
	}

	BMG->FlowSection->createBranchValiditySection();
	BMG->FlowSection->addPadding();
	unsigned long SizeTmp = 0x20 + BMG->InfoSection->SectionSize + BMG->DataSection->SectionSize;
	BMG->Header->FLW1SectionAdr = SizeTmp;
	SizeTmp += BMG->FlowSection->SectionSize;

	// save BCSV
	bcsv->sortAlphabet("MessageId");
	unsigned long size = bcsv->getFileSizeAndAddPadding();

	// swap endian if necessary
	if ( (!pIsLe && strncmp(BMG->mFileHolder.mFileData, "MESG", 4) != 0) || (pIsLe && strncmp(BMG->mFileHolder.mFileData, "MESG", 4) == 0) ) {
		bcsv->swapEndian();
		BMG->Header->swapEndian();
		BMG->InfoSection->swapEndian();
		BMG->DataSection->swapEndian();
		BMG->FlowSection->swapEndian();
	}

	std::ofstream out;
	out.open(mTblFileName, std::ios::out | std::ios::binary);
	out.write((char*)bcsv, size);
	out.close();

	out.open(BMG->mFileHolder.mFileName, std::ios::out | std::ios::binary);
	out.write(BMG->mFileHolder.mFileData, SizeTmp);
	out.close();

	return true;
}

unsigned long BmgWriter::tryAddNewTextData(const wchar_t* pTextData, unsigned long pTextLength) {
	unsigned long Offset;
	bool IsTextEqual;
	//pTextLength *= 2;

	for (Offset = 0; Offset < BMG->DataSection->SectionSize - 8; Offset += 2) {
		wchar_t* BmgTextData = BMG->DataSection->getTextData(Offset);
		IsTextEqual = true;

		// check if the given text can be found
		for (const wchar_t* t = pTextData; t < pTextData + pTextLength; t++) {
			if (*t != *BmgTextData) {
				IsTextEqual = false;
				break;
			}

			BmgTextData++;
		}

		if (IsTextEqual)
			break;
	}

	if (!IsTextEqual) {
		memcpy(BMG->DataSection->getTextData(Offset), pTextData, pTextLength*2);
		BMG->DataSection->SectionSize += pTextLength * 2;
	}

	return Offset;
}

bool BmgWriter::generateTextNode(MESGbmg1::NodeEntryText* pTextNode, XMLEntry& pXmlEntry) {
	char Value[128];
	pTextNode->FlowType = 1;
	pTextNode->Unk1 = 0;
	pTextNode->Unk2 = 0;

	if (pXmlEntry.getEntryInfo("messageID", Value)) {
		unsigned long TextID = 0;

		if (bcsv->getValue(bcsv->findElementString("MessageId", Value), "Index", &TextID))
			pTextNode->TextId = TextID;
		else
			return writeInvalidErrorMessage(Value, pXmlEntry.mEntryStart);
	}
	else 
		return writeMissingErrorMessage("messageID", pXmlEntry.mEntryStart);

	if (pXmlEntry.getEntryInfo("nextNode", Value)) {
		if (strcmp(Value, "none") == 0) {
			// write when "invalid"
			pTextNode->NextFlowId = 0xFFFF;
			pTextNode->Validity = 0xFF;
		}
		else {
			long NextFlowID = XmlUtil::getEntryIndexByEntryNameAndInfo("node", "name", Value, XML);

			if (NextFlowID >= 0) {
				pTextNode->NextFlowId = NextFlowID;
				pTextNode->Validity = 0;
			}
			else
				return writeInvalidErrorMessage(Value, pXmlEntry.mEntryStart);
		}
	}
	else 
		return writeMissingErrorMessage("nextNode", pXmlEntry.mEntryStart);

	return true;
}

bool BmgWriter::generateConditionNode(MESGbmg1::NodeEntryCondition* pConNode, XMLEntry& pXmlEntry) {
	char Value[128];
	pConNode->FlowType = 2;
	pConNode->Unk1 = 2;

	if (pXmlEntry.getEntryInfo("condition", Value))
		pConNode->ConditionType = MESGbmg1::getConditionNodeTypeID(Value);
	else 
		return writeMissingErrorMessage("condition", pXmlEntry.mEntryStart);

	// write argument
	if (pConNode->ConditionType == 0) {
		if (pXmlEntry.getEntryInfo("choice", Value))
			pConNode->Arg = MESGbmg1::getConditionMultipleChoiceID(Value);
		else
			return writeMissingErrorMessage("choice", pXmlEntry.mEntryStart);
	}
	else {
		if (pXmlEntry.getEntryInfo("arg", Value))
			pConNode->Arg = atoi(Value);
		else
			return writeMissingErrorMessage("arg", pXmlEntry.mEntryStart);
	}

	// write BranchNode + add true flow
	if (pXmlEntry.getEntryInfo("trueNode", Value)) {
		pConNode->BranchNodeId = BMG->FlowSection->BranchNodeNum;

		long NextFlowID = 0xFFFF;
		if (strcmp(Value, "none") != 0)
			NextFlowID = XmlUtil::getEntryIndexByEntryNameAndInfo("node", "name", Value, XML);

		if (NextFlowID >= 0)
			addBranchNode(NextFlowID);
		else
			return writeInvalidErrorMessage(Value, pXmlEntry.mEntryStart);
	}
	else
		return writeMissingErrorMessage("trueNode", pXmlEntry.mEntryStart);

	// add false flow
	if (pXmlEntry.getEntryInfo("falseNode", Value)) {
		long NextFlowID = 0xFFFF;
		if (strcmp(Value, "none") != 0)
			NextFlowID = XmlUtil::getEntryIndexByEntryNameAndInfo("node", "name", Value, XML);

		if (NextFlowID >= 0)
			addBranchNode(NextFlowID);
		else
			return writeInvalidErrorMessage(Value, pXmlEntry.mEntryStart);
	}
	else
		return writeMissingErrorMessage("falseNode", pXmlEntry.mEntryStart);

	return true;
}

bool BmgWriter::generateEventNode(MESGbmg1::NodeEntryEvent* pEventNode, XMLEntry& pXmlEntry) {
	char Value[128];
	pEventNode->FlowType = 3;

	if (pXmlEntry.getEntryInfo("event", Value))
		pEventNode->EventType = MESGbmg1::getEventTypeID(Value);
	else 
		return writeMissingErrorMessage("event", pXmlEntry.mEntryStart);

	// write BranchNode
	if (pXmlEntry.getEntryInfo("nextNode", Value)) {
		pEventNode->BranchNodeId = BMG->FlowSection->BranchNodeNum;
		
		long NextFlowID = 0xFFFF;
		if (strcmp(Value, "none") != 0)
			NextFlowID = XmlUtil::getEntryIndexByEntryNameAndInfo("node", "name", Value, XML);

		if (NextFlowID >= 0)
			addBranchNode(NextFlowID);
		else
			return writeInvalidErrorMessage(Value, pXmlEntry.mEntryStart);
	}
	else
		return writeMissingErrorMessage("nextNode", pXmlEntry.mEntryStart);

	// write Argument
	if (pXmlEntry.getEntryInfo("arg", Value))
		pEventNode->Arg = atoi(Value);
	else 
		return writeMissingErrorMessage("arg", pXmlEntry.mEntryStart);

	return true;
}

void BmgWriter::addBranchNode(unsigned long pFlowEntryID) {
	*BMG->FlowSection->getBranchNode(BMG->FlowSection->BranchNodeNum) = pFlowEntryID;
	BMG->FlowSection->BranchNodeNum++;
	BMG->FlowSection->SectionSize += 2;
}

bool BmgWriter::writeMissingErrorMessage(const char* pMissingInfoName, char* pXmlEntryStart) {
	std::cout << "\n" << XmlUtil::countLines(XML->mFileData, pXmlEntryStart) << ": " << pMissingInfoName << " info is missing!\n";
	return false;
}

bool BmgWriter::writeInvalidErrorMessage(const char* pMissingInfoName, char* pXmlEntryStart) {
	std::cout << "\n" << XmlUtil::countLines(XML->mFileData, pXmlEntryStart) << ": " << pMissingInfoName << " is invalid!\n";
	return false;
}





BmgTextGenerator::BmgTextGenerator(const XMLEntry* pXmlParentEntry) {
	mXmlSubEntry = XMLSubEntry(pXmlParentEntry);
	mMessagePos = 0;
}

bool BmgTextGenerator::createMessage() {
	bool isSearchForDataAfterStart = true;
	char* XmlDataParser = mXmlSubEntry.mXmlParentEntry->getEntryData();
	
	if (!XmlDataParser)
		return false;

	while (XmlDataParser < mXmlSubEntry.mXmlParentEntry->mEntryEnd) {
		// skip the first line
		if (!XmlUtil::isValidStringEndChar(*XmlDataParser) && isSearchForDataAfterStart)
			isSearchForDataAfterStart = false;

		if (*XmlDataParser == '\n' && isSearchForDataAfterStart) {
			XmlDataParser++;
			isSearchForDataAfterStart = false;
		}

		if (!isSearchForDataAfterStart && *XmlDataParser != '\r' && *XmlDataParser != '\t') {
			
			// special tag entry
			if (*XmlDataParser == '<') {
				// check for note
				if (mXmlSubEntry.getEntry("note", XmlDataParser))
					writeChar(0x266A);
				else
					createAndAddEventData(XmlDataParser);

				// set the xml parser offset (if an entry has been found)
				if (mXmlSubEntry.mEntryEnd != 0)
					XmlDataParser = mXmlSubEntry.mEntryEnd - 1;
				else
					writeChar((unsigned char)*XmlDataParser);
			}
			else
				writeChar((unsigned char)*XmlDataParser);
		}

		XmlDataParser++;
	}

	// remove 0x000A at the end of message entries
	if (mMessagePos > 0 && mMessageData[mMessagePos-1] == '\n')
		mMessagePos--;

	writeChar(0);

	return true;
}

void BmgTextGenerator::createAndAddEventData(const char* pXmlEntry) {
	unsigned long MessagePosTmp = mMessagePos;
	char TagData[128];

	// check for event
	if (mXmlSubEntry.getEntry("e", pXmlEntry)) {
		// write event type
		mXmlSubEntry.getEntryInfo("type", TagData);
		writeEvent(atoi(TagData), 0);
		char CurrLetterString[2] = "a";
		while (mXmlSubEntry.getEntryInfo(CurrLetterString, TagData)) {
			writeChar(atoi(TagData));
			CurrLetterString[0]++;
		}
		writeEventLength(MessagePosTmp);
	}
	else if (mXmlSubEntry.getEntry("wait", pXmlEntry)) {
		writeEvent(1, 8);				// write event type and size
		writeChar(0);					// write event type type
		if (mXmlSubEntry.getEntryInfo("time", TagData))
			writeChar(atoi(TagData));	// write wait time
		else
			writeChar(30);
	}
	else if (mXmlSubEntry.getEntry("newpage", pXmlEntry)) {
		writeEvent(1, 6);				// write event type and size
		writeChar(1);					// write event type type
	}
	else if (mXmlSubEntry.getEntry("centerText", pXmlEntry)) {
		writeEvent(1, 6);				// write event type and size
		writeChar(3);					// write event type type
	}
	else if (mXmlSubEntry.getEntry("sound", pXmlEntry)) {
		writeEvent(2, 0);				// write event type and size
		writeChar(0);					// write event type type
		if (mXmlSubEntry.getEntryInfo("src", TagData)) {
			// copy sound name into bmg text
			for (unsigned long counter = 0; counter < strlen(TagData); counter++)
				writeSingleChar(TagData[counter], true);
		}
		else
			writeChar(0);
		writeEventLength(MessagePosTmp);
	}
	else if (mXmlSubEntry.getEntry("icon", pXmlEntry)) {
		writeEvent(3, 6);				// write event type and size
		if (mXmlSubEntry.getEntryInfo("src", TagData))
			writeChar(MESGbmg1::getIconNameId(TagData));		// write icon
		else
			writeChar(0);
	}
	else if (mXmlSubEntry.getEntry("font", pXmlEntry)) {
		if (mXmlSubEntry.getEntryInfo("size", TagData)) {
			writeEvent(4, 6);				// write event type and size
			if (strcmp(TagData, "small") == 0) writeChar(0);
			else if (strcmp(TagData, "normal") == 0) writeChar(1);
			else if (strcmp(TagData, "large") == 0) writeChar(2);
			else writeChar(atoi(TagData));
		}
		else if (mXmlSubEntry.getEntryInfo("color", TagData)) {
			writeEvent(255, 8);				// write event type and size
			writeChar(0);					// write event type type
			writeSingleChar(MESGbmg1::getColorNameId(TagData));
		}
		else return;	// skip event entry
	}
	else if (mXmlSubEntry.getEntry("player", pXmlEntry)) {
		writeEvent(5, 8);				// write event type and size
		writeChar(0);					// write event type type
		if (mXmlSubEntry.getEntryInfo("name", TagData)) {
			if (strcmp(TagData, "normal") == 0) writeSingleChar(0);
			else if (strcmp(TagData, "formal") == 0) writeSingleChar(1);
			else if (strcmp(TagData, "moustache") == 0) writeSingleChar(2);
			else writeSingleChar(atoi(TagData));
		}
		else
			writeChar(0);
	}
	else if (mXmlSubEntry.getEntry("valInt", pXmlEntry)) {
		writeEvent(6, 14);
		if (mXmlSubEntry.getEntryInfo("arg1", TagData)) writeChar(atoi(TagData));
		else writeChar(0);
		if (mXmlSubEntry.getEntryInfo("arg2", TagData)) writeLong(atoi(TagData));
		else writeLong(0);
		if (mXmlSubEntry.getEntryInfo("arg3", TagData)) writeLong(atoi(TagData));
		else writeLong(0);
	}
	else if (mXmlSubEntry.getEntry("valStr", pXmlEntry)) {
		writeEvent(7, 14);
		if (mXmlSubEntry.getEntryInfo("arg1", TagData)) writeChar(atoi(TagData));
		else writeChar(0);
		if (mXmlSubEntry.getEntryInfo("arg2", TagData)) writeLong(atoi(TagData));
		else writeLong(0);
		if (mXmlSubEntry.getEntryInfo("arg3", TagData)) writeLong(atoi(TagData));
		else writeLong(0);
	}
}


void BmgTextGenerator::writeChar(wchar_t pChar) {
	mMessageData[mMessagePos] = pChar;
	mMessagePos++;
}

void BmgTextGenerator::writeEvent(unsigned char pEventType, unsigned char pLength) {
	// write event tag start char
	writeChar(0x1A);
	unsigned char* WriteAdr = (unsigned char*)(&mMessageData[mMessagePos]);
	*WriteAdr = pLength;
	WriteAdr++;
	*WriteAdr = pEventType;
	mMessagePos++;
}

void BmgTextGenerator::writeEventLength(unsigned long pEventStartOffset) {
	unsigned char* WriteAdr = (unsigned char*)(&mMessageData[pEventStartOffset + 1]);
	*WriteAdr = (mMessagePos - pEventStartOffset) * 2;
}

void BmgTextGenerator::writeSingleChar(unsigned char pChar, bool pSide) {
	writeChar(0);
	unsigned char* WriteAdr = (unsigned char*)(&mMessageData[mMessagePos - 1]) + pSide;
	*WriteAdr = pChar;
}

void BmgTextGenerator::writeLong(long pLong) {
	long* WriteAdr = (long*)(&mMessageData[mMessagePos]);
	*WriteAdr = pLong;
	mMessagePos += 2;
}
















int commandSearch(int argc, char* argv[], const char* pCommandToSearchFor) {
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], pCommandToSearchFor) == 0)
			return i;
	}

	return -1;
}

int commandSearchValue(int argc, char* argv[], const char* pCommandToSearchFor) {
	int i = commandSearch(argc, argv, pCommandToSearchFor);
	if (i >= 0 && i + 1 < argc)
		return i + 1;
	else
		return -1;
}

int main(int argc, char* argv[]) {
	bool isModeConvertToXML = false;
	bool isOutputLittleEndian = false;
	const char* BmgFileName = "Message.bmg";
	const char* MsgTblFileName = "MessageId.tbl";
	const char* XmlFileName = "Message.xml";
	int BmgFileNameInd = commandSearchValue(argc, argv, "-bmg");
	int MsgTblFileNameInd = commandSearchValue(argc, argv, "-tbl");
	int XmlFileNameInd = commandSearchValue(argc, argv, "-xml");

	if (commandSearch(argc, argv, "-h") >= 0) {
		std::cout << "List of arguments:\n";
		std::cout << "Tomato.exe -bmg2xml -bmg <BMGInputFile> -tbl <TBLInputFile> [-xml <XMLOutputFile>] convert a .bmg / .tbl to .xml\n";
		std::cout << "Tomato.exe -xml2bmg -xml <XMLInputFile> [-bmg <BMGOutputFile>] [-tbl <TBLOutputFile>] [-le (output little endian)] convert an .xml to .bmg / .tbl\n";
		std::cout << "Arguments in square brackets are optional.\n";
		return 0;
	}

	if (commandSearch(argc, argv, "-bmg2xml") >= 0) {
		if (BmgFileNameInd < 0 || MsgTblFileNameInd < 0) {
			std::cout << "Too few or wrong arguments.\nUse -h to view a list of arguments.\n";
			return 0;
		}
		BmgFileName = argv[BmgFileNameInd];
		MsgTblFileName = argv[MsgTblFileNameInd];
		if (XmlFileNameInd >= 0) XmlFileName = argv[XmlFileNameInd];
		isModeConvertToXML = true;
	}
	else if (commandSearch(argc, argv, "-xml2bmg") >= 0) {
		if (XmlFileNameInd < 0) {
			std::cout << "Too few or wrong arguments.\nUse -h to view a list of arguments.\n";
			return 0;
		}
		XmlFileName = argv[XmlFileNameInd];
		if (BmgFileNameInd >= 0) BmgFileName = argv[BmgFileNameInd];
		if (MsgTblFileNameInd >= 0) MsgTblFileName = argv[MsgTblFileNameInd];
		if (commandSearch(argc, argv, "-le") >= 0) isOutputLittleEndian = true;
	}
	else if (argc >= 2) {
		std::cout << "Too few or wrong arguments.\nUse -h to view a list of arguments.\n";
		return 0;
	}
	

	if (isModeConvertToXML) {

		BmgFileHolder* BMG;
		BCSV* bcsv;
		XmlWriter* XML;

		BMG = new BmgFileHolder(BmgFileName);

		if (!BMG->loadBmg()) {
			std::cout << "An error has occured while trying to load the BMG.\n";
			return 0;
		}
	
		std::cout << BmgFileName << " was successfully loaded.\n";

		bcsv = BCSV::loadBcsv(MsgTblFileName);

		if (!bcsv) {
			std::cout << MsgTblFileName << " isn't exist.\n";
			return 0;
		}

		std::cout << MsgTblFileName << " was successfully loaded.\n";

		XML = new XmlWriter(XmlFileName, BMG, bcsv);
		std::cout << "Starting conversion to XML...\n";

		if (!XML->writeXml()) {
			std::cout << "An error has occured while trying to generate the XML.\n";
			return 0;
		}

		std::cout << "\nFinished!\n";
	}
	else {
		BmgWriter BmgWr = BmgWriter(XmlFileName, BmgFileName, MsgTblFileName);

		if (!BmgWr.writeBmg(isOutputLittleEndian))
			std::cout << "An error occured while trying to convert the XML to BMG.\n";
		else {
			std::cout << "\nFinished!\n";
		}
	}

	return 0;
}