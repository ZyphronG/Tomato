#pragma once

namespace MESGbmg1 {

    // Base Util
    const char* getElementFromTable(const char** pTable, unsigned long pNumOfElements, unsigned long pIndex);
    unsigned long getElementIndexFromTable(const char** pTable, unsigned long pNumOfElements, const char* pElementToSearchFor);

    // message util
    const char* getIconName(unsigned long);
	unsigned long getIconNameId(const char* pIconName);
	const char* getColorName(unsigned long);
	unsigned long getColorNameId(const char* pFontColorName);
	const char* getSoundIDName(unsigned char);
	unsigned long getSoundNameIDID(const char* pSoundIDName);
    const char* getMessageTypeName(unsigned char);
    unsigned long getMessageTypeNameID(const char* pMessageTypeName);
    const char* getCamTypeName(unsigned char);
    unsigned long getCamTypeNameID(const char* pCamTypeName);
    const char* getTextBoxTypeName(unsigned char);
    unsigned long getTextBoxTypeNameID(const char* pTextBoxTypeName);

    // flow util
    const char* getConditionNodeType(unsigned long);
    unsigned long getConditionNodeTypeID(const char*);
    const char* getConditionMultipleChoice(unsigned long);
    unsigned long getConditionMultipleChoiceID(const char*);
    const char* getEventType(unsigned long);
    unsigned long getEventTypeID(const char*);


    // documenting and writing these lists was a pain.
    // please do not steal them without crediting me
    
    static const char* IconNameTable[] = {
        "a_button",	//0
        "b_button",	//1
        "c_button",	//2
        "wiimote",	//3
        "nunchuck",	//4
        "1_button",	//5
        "2_button",	//6
        "powerstar",	//7
        "spindriver",	//8
        "pullstar",	//9
        "pointer_p1",	//10
        "starbit_purple",	//11
        "coconut",	//12
        "bell",	//13
        "starbunny",	//14
        "joystick",	//15
        "x",	//16
        "coin",	//17
        "mario",	//18
        "d-pad",	//19
        "bluechip",	//20
        "yellowchip",	//21
        "home_button",	//22
        "minus_button",	//23
        "plus_button",	//24
        "z_button",	//25
        "silverstar",	//26
        "grandstar",	//27
        "luigi",	//28
        "pointer_p2",	//29
        "purplecoin",	//30
        "greenstar",	//31
        "crown",	//32
        "crosshair",	//33
        "blank",	//34
        "redstar",	//35
        "pointer_drag",	//36
        "pointer_point",	//37
        "pointer_grab",	//38
        "starbit",	//39
        "peach",	//40
        "letter",	//41
        "42",	//42
        "mario_or_luigi",	//43
        "44",	//44
        "45",	//45
        "46",	//46
        "47",	//47
        "questionmark",	//48
        "one-up",	//49
        "life-up",	//50
        "hungryluma",	//51
        "polari",	//52
        "cometstar",	//53
        "questionmark_green"	//54
    };

    static const char* FontColorNames[] = {
        "black",
        "red",
        "green",
        "blue",
        "yellow",
        "purple",
        "lightred",
        "lightgreen",
        "lightblue",
        "lightyellow",
        "brightpink",
        "gray"
    };

    static const char* SoundIDs[] = {
        "null",	//0
        "SE_SV_KINOPIO_TALK_HEY",	//1
        "SE_SV_KINOPIO_TALK_YAHOO",	//2
        "SE_SV_KINOPIO_TALK_ANGRY",	//3
        "SE_SV_KINOPIO_TALK_SAD",	//4
        "SE_SV_KINOPIO_TALK_HAPPY",	//5
        "SE_SV_KINOPIO_TALK_SLEEP",	//6
        "SE_SV_RABBIT_TALK_NORMAL",	//7
        "SE_SV_RABBIT_TALK_CAUGHT",	//8
        "SE_SV_RABBIT_TALK_THATS",	//9
        "SE_SV_RABBIT_TALK_HELP",	//10
        "SE_SV_RABBIT_TALK_THANKS",	//11
        "SE_SV_PENGUIN_L_TALK_NORMAL",	//12
        "SE_SV_PENGUIN_L_TALK_PLEASED",	//13
        "SE_SV_PENGUIN_L_TALK_NG",	//14
        "SE_SV_PENGUIN_L_TALK_QUESTION",	//15
        "SE_SV_PENGUIN_L_TALK_DISTANT",	//16
        "SE_SV_PENGUIN_L_TALK_NORMAL_L",	//17
        "SE_SV_PENGUIN_L_TALK_PLEASED_L",	//18
        "SE_SV_PENGUIN_L_TALK_NG_L",	//19
        "SE_SV_PENGUIN_L_TALK_QUESTION_L",	//20
        "SE_SV_PENGUIN_L_TALK_OH",	//21
        "SE_SV_PENGUIN_S_TALK_NORMAL",	//22
        "SE_SV_PENGUIN_S_TALK_GLAD",	//23
        "SE_SV_PENGUIN_S_TALK_GLAD_HIGH",	//24
        "SE_SV_PENGUIN_S_TALK_ANGRY",	//25
        "SE_SV_PENGUIN_S_TALK_SAD",	//26
        "SE_SV_PENGUIN_S_TALK_HAPPY",	//27
        "SE_SV_PENGUIN_S_TALK_STRONG",	//28
        "SE_SV_PENGUIN_S_TALK_NORMAL_W",	//29
        "SE_SV_PENGUIN_S_TALK_GREET",	//30
        "SE_SV_PENGUIN_S_TALK_WIN",	//31
        "SE_SV_PENGUIN_S_TALK_LOSE",	//32
        "SE_SV_PENGUIN_S_TALK_OUCH",	//33
        "SE_SV_PENGUIN_ACE_TALK_NORMAL",	//34
        "SE_SV_PENGUIN_ACE_TALK_GREET",	//35
        "SE_SV_PENGUIN_ACE_TALK_WIN",	//36
        "SE_SV_PENGUIN_ACE_TALK_LOSE",	//37
        "SE_SV_SYATI_TALK_NORMAL",	//38
        "SE_SV_SYATI_TALK_RING_MEET",	//39
        "SE_SV_SYATI_TALK_RING_WIN",	//40
        "SE_SV_SYATI_TALK_RING_LOSE",	//41
        "SE_SV_SYATI_TALK_PHAN_MEET",	//42
        "SE_SV_SYATI_TALK_PHAN_WIN",	//43
        "SE_SV_SYATI_TALK_PHAN_LOSE",	//44
        "SE_SV_PENGUIN_SS_HAPPY",	//45
        "SE_SV_PENGUIN_SS_GREET",	//46
        "SE_SV_PENGUIN_SS_DAMAGE",	//47
        "SE_SV_PENGUIN_SS_DISAPPOINTED",	//48
        "SE_SV_PENGUIN_SS_PLEASED",	//49
        "SE_SV_PENGUIN_SS_ANGRY",	//50
        "SE_SV_HONEYBEE_TALK_NORMAL",	//51
        "SE_SV_HONEYBEE_TALK_CONFUSION",	//52
        "SE_SV_HONEYBEE_TALK_QUESTION",	//53
        "SE_SV_HONEYBEE_TALK_SURPRISE",	//54
        "SE_SV_HONEYBEE_TALK_ORDER",	//55
        "SE_SV_HONEYBEE_TALK_LAUGH",	//56
        "SE_SV_HONEYBEE_TALK_GLAD",	//57
        "SE_SV_TICO_TALK_NORMAL",	//58
        "SE_SV_TICO_TALK_GLAD",	//59
        "SE_SV_TICO_TALK_ANGRY",	//60
        "SE_SV_TICO_TALK_SAD",	//61
        "SE_SV_TICO_TALK_HAPPY",	//62
        "null_1",	//63
        "SE_SV_TICO_TICO",	//64
        "SE_SV_TICO_TALK_CONFUSION",	//65
        "SE_SV_TICO_TALK_THANKS",	//66
        "SE_SV_LUIGI_MARIO",	//67
        "SE_SV_LUIGI_FRIGHTENED",	//68
        "SE_SV_LUIGI_SORRY",	//69
        "SE_SV_LUIGI_THANKS",	//70
        "SE_SV_LUIGI_HEY",	//71
        "SE_DM_KINOPIO_CHIEF",	//72
        "SE_SV_TERERACER_TALK_NORMAL",	//73
        "SE_SV_TERERACER_TALK_LAUGH",	//74
        "SE_SV_TERERACER_TALK_REGRET",	//75
        "SE_SV_CARETAKER_SHORT",	//76
        "SE_SV_CARETAKER_NORMAL",	//77
        "SE_SV_CARETAKER_LONG",	//78
        "SE_SV_CARETAKER_REPEAT",	//79
        "null_2",	//80
        "null_3",	//81
        "null_4",	//82
        "null_5",	//83
        "null_6",	//84
        "SE_SV_HONEYQUEEN_TALK_SURPRISE",	//85
        "SE_SV_HONEYQUEEN_TALK_THANKS",	//86
        "SE_SV_HONEYQUEEN_TALK_WORRY",	//87
        "SE_SV_HONEYQUEEN_TALK_AA",	//88
        "SE_SV_HONEYQUEEN_TALK_AN",	//89
        "SE_SV_HONEYQUEEN_TALK_UFUFU",	//90
        "SE_SV_KINOPIO_TALK_WELCOME",	//91
        "SE_SV_KINOPIO_TALK_BEAUTIFUL",	//92
        "SE_SV_KINOPIO_TALK_SURPRISE",	//93
        "SE_SV_KINOPIO_PUHA",	//94
        "SE_SV_KINOPIO_TALK_HELP",	//95
        "SE_SV_KINOPIO_TALK_TREMBLE",	//96
        "SE_SV_KINOPIO_TALK_STRONG",	//97
        "SE_SV_KINOPIO_TALK_LOOK_OUT",	//98
        "SE_SV_KINOPIO_TALK_WOW",	//99
        "SE_SV_PEACH_TALK_HELP",	//100
        "SE_SV_ROSETTA_TALK_NORMAL",	//101
        "SE_SV_ROSETTA_TALK_SURPRISE",	//102
        "SE_SV_ROSETTA_TALK_SMILE",	//103
        "SE_SV_ROSETTA_TALK_WAITING",	//104
        "SE_SV_ROSETTA_TALK_WORRIED",	//105
        "SE_SV_TICOFAT_TALK_NORMAL",	//106
        "SE_SV_TICOFAT_TALK_KITA",	//107
        "SE_SV_TICOFAT_META",	//108
        "SE_SV_KINOPIOCHIEF_TALK_HEY",	//109
        "SE_SV_KINOPIOCHIEF_TALK_LAUGH",	//110
        "SE_SV_KINOPIOCHIEF_TALK_YAHOO",	//111
        "SE_SV_TICOFAT_TALK_GIVE_ME",	//112
        "SE_SV_TICOFAT_TALK_WAKU",	//113
        "SE_SV_BUTLER_TALK_SURPRISE",	//114
        "SE_SV_BUTLER_TALK_AGREE",	//115
        "SE_SV_BUTLER_TALK_WORRIED",	//116
        "SE_SV_BUTLER_TALK_NORMAL",	//117
        "SE_SV_PENGUIN_OLD_GREET",	//118
        "SE_SV_PENGUIN_OLD_GRAD",	//119
        "SE_SV_PENGUIN_OLD_SAD",	//120
        "SE_SV_PENGUIN_OLD_NORMAL",	//121
        "SE_SV_LUIGI_TALK_TIRE",	//122
        "SE_SV_LUIGI_TALK_YAH",	//123
        "SE_SV_LUIGI_TALK_HELP",	//124
        "SE_SV_LUIGI_TALK_OH_YEAH",	//125
        "SE_SV_BUTLER_TALK_QUESTION",	//126
        "SE_SV_ROSETTA_TALK_LETS_START",	//127
        "SE_SV_ROSETTA_TALK_SOMUCH_TODAY",	//128
        "SE_SV_PENGUIN_OLD_SCARED",	//129
        "SE_SV_TICOCOMET_TALK_PURURIN",	//130
        "SE_SV_TICOCOMET_TALK_DON",	//131
        "SE_SV_KINOPIO_TALK_WATER",	//132
        "SE_SV_KINOPIO_TALK_HEY_SNOR",	//133
        "SE_SV_KINOPIO_TALK_SAD_SNOR",	//134
        "SE_SV_TICOSHOP_TALK_PIKARIN",	//135
        "SE_SV_TICOSHOP_TALK_KITA",	//136
        "SE_BV_KOOPAJR_TLK_PROVOKE",	//137
        "SE_BV_KOOPA_TLK_LAUGH",	//138
        "SE_BV_KOOPA_TLK_NORMAL",	//139
        "SE_BV_KOOPA_TLK_REGRET",	//140
        "SE_BV_KOOPA_TLK_CALM",	//141
        "SE_BV_KOOPA_TLK_EXCITED",	//142
        "SE_SV_TICOFAT_TALK_YEAH",	//143
        "SE_SV_ROSETTA_TALK_THATS_ALL",	//144
        "SE_SV_CARE_TAKER_TRAMPLE",	//145
        "SE_SV_KINOPIO_NO_MAIL",	//146
        "SE_SV_KINOPIO_LOOK_MAIL",	//147
        "SE_SV_KINOPIO_TALK_SHOUT",	//148
        "SE_SV_KINOPIOCHIEF_TALK_EVASIVE",	//149
        "SE_SV_ROSETTA_TALK_SO",	//150
        "SE_SV_ROSETTA_TALK_LOOK",	//151
        "SE_SM_SIGNBOARD_HEY",	//152
        "SE_SV_ROSETTA_TALK_PLEASE",	//153
        "SE_SV_ROSETTA_TALK_FU",	//154
        "SE_SV_ROSETTA_TALK_TROUBLE",	//155
        "SE_SV_ROSETTA_TALK_SIGH",	//156
        "SE_SV_ROSETTA_TALK_QUESTION",	//157
        "SE_SV_ROSETTA_TALK_SMILE_EP_B",	//158
        "SE_SV_HONEYBEE_TALK_SLEEP",	//159
        "SE_SV_KINOPIO_TALK_TIRED",	//160
        "SE_SV_CARETAKER_ANGRY_FAST"	//161
    };

    static const char* NodeConditionTypeList[] = {
        "multipleChoice",   //0
        "coded",    //1
        "playerNearNpc",  //2
        "SW_A",   //3
        "SW_B",   //4
        "playerStateNoPowerUp",    //5
        "playerStateBee", //6
        "playerStateBoo", //7
        "powerStarSpawned",   //8
        "alreadyTalkedScene",  //9
        "playerLuigi",   //10
        "GetBranchAstroGalaxyResult",   //11
        "cutsceneActive",    //12
        "alreadyTalkedSaved",    //13
        "IsMsgLedPattern"   //14, Korean only (i think)
    };

    static const char* NodeConditionMultipleChoices[] = {
        "PenguinRace", //0
        "SwimmingSchool", //1
        "PenguinRace_alt", //2
        "BombTimeAttackLv1", //3
        "PhantomTeresaRacer", //4
        "BombTimeAttackLv2", //5
        "TrialSurfingCoach", //6
        "TrialSurfingHowTo", //7
        "DeathPromenadeTeresaRacer", //8
        "RosettaFinalBattle", //9
        "CometTico", //10
        "TransformTico", //11
        "ChallengeSurfingCoach", //12
        "TicoShopExchange", //13
        "TicoShopWhich", //14
        "KinopioPurple", //15
        "CometTicoTell", //16
        "TrialTamakoroHowTo", //17
        "KnockOnTheDoor", //18
        "LedPattern" //19, Korean only
    };

    static const char* NodeEventTypes[] = {
        "npcEvent", //0
        "npcEvent_alt", //1
        "nextText", //2
        "unk_3", //3
        "emotion", //4
        "SW_A", //5
        "SW_B", //6
        "metamorphosis" //7
    };




    class FileHeader {
    public:

        void swapEndian();
        void setUp();

	    unsigned long FileHeaderMagic;	//_0
        unsigned long SectionMagic;  //_4
	    unsigned long FLW1SectionAdr;	//_8
	    unsigned long SectionNum;	//_C
	    unsigned char unk;	//_10 unknown but usually 2
	    char Padding[15];	//_11
    };

    class SectionBase {
    public:

        void swapEndian();
        void addPadding();

        unsigned long Magic;    //_0
        unsigned long SectionSize;  //_4
    };




    struct INF1Entry {
        unsigned long TextAddress;  //_0
        unsigned short CameraId;    //_4
        unsigned char SoundId;  //_6
        unsigned char CamType;    //_7; 0 = false, 1 = use given CameraId
        unsigned char MessageType; //_8; 0 = Talk, 1 = Shout, 2 = auto start Talk, 3 = unknow / game crash, 4 = Empty Message (used for flows)
        unsigned char MessageBoxType; //_9; 0 = normal, 4 = Sign Board
        unsigned char MessageAreaId;    //_A; 255 = invalid
        unsigned char Unknown;  //_B seems to be always 255
    };

    class INF1 : public SectionBase {
    public:

        void swapEndian();
        INF1Entry* getInfoData(unsigned long IndexID);
        void setUp(unsigned long pEntryNum);

        unsigned short EntryNum;    //_8
        unsigned short EntrySize;   //_A
        char Padding[4];    //_C
    };




    class DAT1 : public SectionBase {
    public:

        void swapEndian();
        wchar_t* getTextData(unsigned long Offset);
        void setUp();
    };




    struct NodeEntryText {
        unsigned char FlowType;     //_0
        unsigned char Unk1;         //_1
        unsigned short TextId;      //_2
        unsigned short NextFlowId;  //_4
        unsigned char Validity;     //_6
        unsigned char Unk2;         //_7
    };

    struct NodeEntryCondition {
        unsigned char FlowType;         //_0
        unsigned char Unk1;             //_1
        unsigned short ConditionType;   //_2
        unsigned short Arg;             //_4
        unsigned short BranchNodeId;    //_6
    };

    struct NodeEntryEvent {
        unsigned char FlowType;         //_0
        unsigned char EventType;        //_1
        unsigned short BranchNodeId;    //_2
        unsigned long Arg;              //_4
    };

    class FLW1 : public SectionBase {
    public:

        void swapEndian();
        void setUp();
        NodeEntryText* getNodeEntry(unsigned long Index);
        unsigned short* getBranchNode(unsigned long Index);
        char* getBranchNodeValid(unsigned long Index);
        void createBranchValiditySection();

        unsigned short NodeNum;    //_8
        unsigned short BranchNodeNum;   //_A
        char Padding[4];    //_C
    };




    class FLI1 : public SectionBase {
    public:

        unsigned short EntryNum;    //_8
        unsigned char EntryLength;  //_A
        char Padding[5];    //_B
    };
}