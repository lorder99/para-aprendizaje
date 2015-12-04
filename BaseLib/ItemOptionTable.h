#pragma once

#include "Table.h"
#include "NtlItem.h"

const DWORD		DBO_MAX_LENGTH_ITEM_OPTION_NAME = 30;
const DWORD		DBO_MAX_LENGTH_ITEM_OPTION_NOTE = 128;

#pragma pack(push, 4)
struct sITEM_OPTION_TBLDAT : public sTBLDAT
{
	sITEM_OPTION_TBLDAT()
	{
		bValidity_Able = true;
	};

public:
	WCHAR			wszOption_Name[DBO_MAX_LENGTH_ITEM_OPTION_NAME + 1];
	bool			bValidity_Able;
	BYTE			byOption_Rank;
	BYTE			byItem_Group;
	BYTE			byMaxQuality;		// �ش� �׷��� ����Ƽ�� �Ѽ�
	BYTE			byQuality;			// �̴����̺� �ִ� ����Ƽ
	BYTE			byQualityIndex;		// ���° ����Ƽ
	DWORD			dwCost;
	BYTE			byLevel;
	TBLIDX			system_Effect[NTL_MAX_SYSTEM_EFFECT_COUNT];
	bool			bAppliedInPercent[NTL_MAX_SYSTEM_EFFECT_COUNT];
	int				nValue[NTL_MAX_SYSTEM_EFFECT_COUNT];

	TBLIDX			activeEffect;
	float			fActiveRate;
	char			szNote[DBO_MAX_LENGTH_ITEM_OPTION_NOTE + 1];
	BYTE			byScouterInfo;   // cf) Defined by eSCOUTER_INFO of NtlItem.h 

public:

	virtual int GetDataSize()
	{
		return sizeof(*this) - sizeof(void*);
	}
};
#pragma pack(pop)

class ItemOptionTable : public Table
{
public:

	ItemOptionTable(void);
	virtual ~ItemOptionTable(void);

	bool Create(DWORD dwCodePage);
	void Destroy();

	sTBLDAT* FindData(TBLIDX tblidx);
	//sTBLDAT* GetOptionTbldat(BYTE byType, BYTE byDefLevel, BYTE byRank, BYTE byCount);
	//bool SetOptionTbldat(BYTE byType, BYTE byDefLevel, BYTE byRank, BYTE byCount, sTBLDAT * pTbldat);

	virtual bool LoadFromBinary(Serializer& serializer, bool bReload);
	virtual bool SaveToBinary(Serializer& serializer);

protected:
	void Init();

	WCHAR** GetSheetListInWChar() { return &(ItemOptionTable::m_pwszSheetList[0]); }
	void* AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage);
	bool DeallocNewTable(void* pvTable, WCHAR* pwszSheetName);
	bool AddTable(void * pvTable, bool bReload);
	bool SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData);

private:
	static WCHAR* m_pwszSheetList[];
};