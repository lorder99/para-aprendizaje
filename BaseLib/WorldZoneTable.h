#pragma once

#include "Table.h"

const DWORD		DBO_MAX_LENGTH_WORLD_ZONE_NAME = 40;

#pragma pack(push, 4)
struct sWORLD_ZONE_TBLDAT : public sTBLDAT
{
public:
	WORD			wFunctionBitFlag;
	TBLIDX			worldTblidx;
	TBLIDX			nameTblidx;
	WCHAR			wszName_Text[DBO_MAX_LENGTH_WORLD_ZONE_NAME + 1];
	bool			bForbidden_Vehicle;

protected:

	virtual int GetDataSize()
	{
		return sizeof(*this) - sizeof(void*);
	}
};
#pragma pack(pop)

class WorldZoneTable : public Table
{
public:
	WorldZoneTable(void);
	virtual ~WorldZoneTable(void);

	bool Create(DWORD dwCodePage);
	void Destroy();

	sTBLDAT* FindData(TBLIDX tblidx);

	virtual bool LoadFromBinary(Serializer& serializer, bool bReload);
	virtual bool SaveToBinary(Serializer& serializer);

protected:
	void Init();
	WCHAR** GetSheetListInWChar() { return &(WorldZoneTable::m_pwszSheetList[0]); }
	void* AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage);
	bool DeallocNewTable(void* pvTable, WCHAR* pwszSheetName);
	bool AddTable(void * pvTable, bool bReload);
	bool SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData);

private:
	static WCHAR* m_pwszSheetList[];
};
