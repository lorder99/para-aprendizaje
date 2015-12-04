#pragma once

#include "Table.h"
#include "NPCTable.h"

enum eMOB_PROPERTY_TYPE
{
	MOB_PROPERTY_DUNGEON_BOSS,
	MOB_PROPERTY_NONE = 0xFF
};

// MOB
#pragma pack(push, 4)
struct sMOB_TBLDAT : public sBOT_TBLDAT
{
public:
	DWORD			dwMobGroup;
	WORD			wMob_Kind;
	DWORD			dwDrop_Zenny;
	float			fDrop_Zenny_Rate;
	WORD			wExp;
	BYTE			byGrade;
	BYTE			byMob_Type;
	TBLIDX			drop_Item_Tblidx;

	bool			bSize;
	WORD			wTMQPoint;

	TBLIDX			dropQuestTblidx;
	TBLIDX			dropTypeTblidx;
	TBLIDX			dropEachTblidx;

	BYTE			byDropTypeRateControl;
	BYTE			byDropEachRateControl;

	BYTE			byDropNItemRateControl;
	BYTE			byDropSItemRateControl;
	BYTE			byDropEItemRateControl;
	BYTE			byDropLItemRateControl;

	bool			bShow_Name;

	BYTE			byProperty;
	BYTE			byHtbBlockRate;

	WORD			wSightAngle;

public:

	virtual int GetDataSize()
	{
		return sizeof(*this) - sizeof(void*);
	}
};
#pragma pack(pop)

class MobTable : public Table
{
public:
	MobTable(void);
	virtual ~MobTable(void);

	bool Create(DWORD dwCodePage);
	void Destroy();

	typedef std::map<DWORD, TBLIDX> MOB_TABLE;
	typedef MOB_TABLE::iterator MOB_TABLEIT;
	typedef MOB_TABLE::value_type MOB_TABLEVAL;

	MOB_TABLEIT	MobBegin() { return m_mapMobTableList.begin(); }
	MOB_TABLEIT	MobEnd() { return m_mapMobTableList.end(); }

	sTBLDAT* FindData(TBLIDX tblidx);
	TBLIDX FindTblidxByGroup(DWORD dwMobGroup);

	virtual bool LoadFromBinary(Serializer& serializer, bool bReload);
	virtual bool SaveToBinary(Serializer& serializer);

protected:
	void Init();

	WCHAR** GetSheetListInWChar() { return &(MobTable::m_pwszSheetList[0]); }
	void* AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage);
	bool DeallocNewTable(void* pvTable, WCHAR* pwszSheetName);
	bool AddTable(void * pvTable, bool bReload);
	bool SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData);

private:
	static WCHAR* m_pwszSheetList[];
	MOB_TABLE			m_mapMobTableList;
};
