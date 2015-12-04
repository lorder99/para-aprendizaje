#pragma once

#include "Table.h"
#include "NtlVector.h"

#pragma pack(push, 4)
struct sSPAWN_TBLDAT : public sTBLDAT
{
public:

	sSPAWN_TBLDAT()
		: spawnGroupId(INVALID_SPAWNGROUPID)
		, byWander_Range(INVALID_BYTE)
	{
	}

	TBLIDX			mob_Tblidx;

	CNtlVector		vSpawn_Loc;
	CNtlVector		vSpawn_Dir;

	BYTE			bySpawn_Loc_Range;
	BYTE			bySpawn_Quantity;
	WORD			wSpawn_Cool_Time;
	BYTE			bySpawn_Move_Type;

	BYTE			byWander_Range;
	BYTE			byMove_Range;
	BYTE			byMove_DelayTime;

	TBLIDX			path_Table_Index;
	CNtlVector		vFollowDistance;

	TBLIDX			playScript;
	TBLIDX			playScriptScene;

	TBLIDX			aiScript;
	TBLIDX			aiScriptScene;

	DWORD			dwParty_Index;
	bool			bParty_Leader;

	SPAWNGROUPID	spawnGroupId;

protected:

	virtual int GetDataSize()
	{
		return sizeof(*this) - sizeof(void*);
	}
};
#pragma pack(pop)

class SpawnTable : public Table
{
public:
	SpawnTable();
	virtual ~SpawnTable();

	bool Create(DWORD dwCodePage);
	void Destroy();

	sTBLDAT* FindData(TBLIDX tblidx);

	virtual bool LoadFromBinary(Serializer& serializer, bool bReload);
	virtual bool SaveToBinary(Serializer& serializer);

	sSPAWN_TBLDAT* GetSpawnGroupFirst(SPAWNGROUPID spawnGroupId);
	sSPAWN_TBLDAT* GetSpawnGroupNext(SPAWNGROUPID spawnGroupId);
	DWORD GetSpawnGroupCount(SPAWNGROUPID spawnGroupId) { return (DWORD)m_spawnGroup.count(spawnGroupId); }

protected:
	void Init();

	WCHAR** GetSheetListInWChar() { return &(SpawnTable::m_pwszSheetList[0]); }
	void* AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage);
	bool DeallocNewTable(void* pvTable, WCHAR* pwszSheetName);
	bool AddTable(void * pvTable, bool bReload);
	bool SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData);

private:
	static WCHAR* m_pwszSheetList[];

	typedef std::multimap<SPAWNGROUPID, sSPAWN_TBLDAT*> SPAWNGROUP;
	typedef SPAWNGROUP::iterator SPAWNGROUP_IT;
	typedef SPAWNGROUP::value_type SPAWNGROUP_VAL;

	SPAWNGROUP m_spawnGroup;
	SPAWNGROUP_IT m_spawnGroupItBeg;
	SPAWNGROUP_IT m_spawnGroupItEnd;
};
