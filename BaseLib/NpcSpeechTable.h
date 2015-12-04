#pragma once

#include "Table.h"
#include "NtlVector.h"

#include <vector>

const DWORD		DBO_MAX_LENGTH_SPEECH_NOTE = 32;
const DWORD		DBO_MAX_LENGTH_DIALOG_TYPE = 16;

const BYTE		DBO_MIN_SPEECH_RANDOM_VALUE = 1;
const BYTE		DBO_MAX_SPEECH_RANDOM_VALUE = 100;

enum eNPC_SPEECH_TYPE
{
	NPC_SPEECH_TYPE_FAINT,
	NPC_SPEECH_TYPE_IDLE,
	NPC_SPEECH_TYPE_FIGHTING,

	MAX_NPC_SPEECH_TYPE,
	INVALID_NPC_SPEECH_TYPE = 0xFF,
};

struct sSPEECH_DATA
{
	sSPEECH_DATA() :
		bySpeechProb(0), speechTblidx(INVALID_TBLIDX) {}

	BYTE					bySpeechProb;
	TBLIDX					speechTblidx;
};

struct sSPEECH_GROUP_DATA
{
	sSPEECH_GROUP_DATA() :
		byNonSpeechProb(0) {}

	typedef std::vector<sSPEECH_DATA> LIST;
	typedef LIST::iterator LIST_IT;

	BYTE					byNonSpeechProb;
	LIST					speechDataList;
};

struct sNPC_SPEECH_GROUP
{
	sSPEECH_GROUP_DATA		asSpeechGroupDataData[MAX_NPC_SPEECH_TYPE];
	bool					SetSpeechData(eNPC_SPEECH_TYPE eSpeechType, BYTE byProbValue, TBLIDX speechTblidx);
	TBLIDX					GetSpeechData(eNPC_SPEECH_TYPE eSpeechType, BYTE byProbValue);
	bool					HasSpeechData(eNPC_SPEECH_TYPE eSpeechType);
};

#pragma pack(push, 4)

struct sNPC_SPEECH_TBLDAT : public sTBLDAT
{
public:

	DWORD				dwDialogGroup;
	char				szDialogType[DBO_MAX_LENGTH_DIALOG_TYPE + 1];
	BYTE				byRate;
	TBLIDX				textIndex;
	BYTE				byBallonType;
	DWORD				dwDisplayTime;
	char				szNote[DBO_MAX_LENGTH_SPEECH_NOTE + 1];
	BYTE				bySpeechType; // eNPC_SPEECH_TYPE

protected:
	virtual int GetDataSize()
	{
		return sizeof(*this) - sizeof(void*);
	}
};

#pragma pack(pop)
//-----------------------------------------------------------------------------------


class NpcSpeechTable : public Table
{
public:
	NpcSpeechTable(void);
	virtual ~NpcSpeechTable(void);

	virtual bool LoadFromBinary(Serializer& serializer, bool bReload);
	virtual bool SaveToBinary(Serializer& serializer);
	bool Create(DWORD dwCodePage);
	void Destroy();

	sTBLDAT* FindData(TBLIDX tblidx);
	bool AddSpeechGroup(sNPC_SPEECH_TBLDAT* pNpcSpeechTbldat);
	sNPC_SPEECH_GROUP* FindSpeechGroup(DWORD dwSpeechGroup);

protected:
	void Init();

	WCHAR** GetSheetListInWChar() { return &(NpcSpeechTable::m_pwszSheetList[0]); }
	void* AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage);
	bool DeallocNewTable(void* pvTable, WCHAR* pwszSheetName);
	bool AddTable(void * pvTable, bool bReload);
	bool SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData);

private:
	typedef std::map<DWORD, sNPC_SPEECH_GROUP*> SPEECH_GROUP;
	typedef SPEECH_GROUP::value_type SPEECH_GROUP_VAL;
	typedef SPEECH_GROUP::iterator SPEECH_GROUP_IT;
	SPEECH_GROUP m_speechGroupList;

	static WCHAR* m_pwszSheetList[];

};