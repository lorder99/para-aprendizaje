#pragma once

#include "Table.h"

const DWORD		DBO_MAX_LENGTH_FILE = 64;

#pragma pack(push, 4)
struct sSCRIPT_LINK_TBLDAT : public sTBLDAT
{
public:
	WCHAR			wszValue[DBO_MAX_LENGTH_FILE + 1];
	BYTE			byType;
	BYTE			byAction;

protected:
	virtual int GetDataSize()
	{
		return sizeof(*this) - sizeof(void*);
	}
};
#pragma pack(pop)

class ScriptLinkTable : public Table
{
public:
	ScriptLinkTable(void);
	virtual ~ScriptLinkTable(void);

	bool Create(DWORD dwCodePage);
	void Destroy();

	sTBLDAT* FindData(TBLIDX tblidx);

	virtual bool LoadFromBinary(Serializer& serializer, bool bReload);
	virtual bool SaveToBinary(Serializer& serializer);

protected:
	void Init();

	WCHAR** GetSheetListInWChar() { return &(ScriptLinkTable::m_pwszSheetList[0]); }
	void* AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage);
	bool DeallocNewTable(void* pvTable, WCHAR* pwszSheetName);
	bool AddTable(void * pvTable, bool bReload);
	bool SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData);

private:
	static WCHAR* m_pwszSheetList[];
};