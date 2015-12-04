#include "stdafx.h"
#include "NewbieTable.h"
#include "NtlCharacter.h"
#include "Serializer.h"

WCHAR* NewbieTable::m_pwszSheetList[] =
{
	L"Table_Data_KOR",
	NULL
};
NewbieTable::NewbieTable(void)
{
	Init();
}

NewbieTable::~NewbieTable(void)
{
	Destroy();
}

bool NewbieTable::Create(DWORD dwCodePage)
{
	return Table::Create(dwCodePage);
}

void NewbieTable::Destroy()
{
	::ZeroMemory(m_aNewbieTbldat, sizeof(m_aNewbieTbldat));

	Table::Destroy();
}

void NewbieTable::Init()
{
	ZeroMemory(m_aNewbieTbldat, sizeof(m_aNewbieTbldat));
}

void* NewbieTable::AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sNEWBIE_TBLDAT* pNewSpawn = new sNEWBIE_TBLDAT;
		if (NULL == pNewSpawn)
			return NULL;

		CPINFO cpInfo;
		if (false == GetCPInfo(dwCodePage, &cpInfo))
		{
			delete pNewSpawn;
			return NULL;
		}

		m_dwCodePage = dwCodePage;
		return pNewSpawn;
	}

	return NULL;
}

bool NewbieTable::DeallocNewTable(void* pvTable, WCHAR* pwszSheetName)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sNEWBIE_TBLDAT* pNewbie = (sNEWBIE_TBLDAT*)pvTable;
		if (FALSE != IsBadReadPtr(pNewbie, sizeof(*pNewbie)))
			return false;

		delete pNewbie;

		return true;
	}

	return false;
}

bool NewbieTable::AddTable(void * pvTable, bool bReload)
{
	UNREFERENCED_PARAMETER(bReload);

	sNEWBIE_TBLDAT* pNewbie = (sNEWBIE_TBLDAT*)pvTable;

	if (CNtlVector::ZERO == pNewbie->vSpawn_Dir)
	{
		pNewbie->vSpawn_Dir = CNtlVector::UNIT_X;
	}
	else
	{
		if (false == (pNewbie->vSpawn_Dir).SafeNormalize())
		{
			_ASSERTE(0);
			return false;
		}
	}

	if (CNtlVector::ZERO == pNewbie->vBind_Dir)
	{
		pNewbie->vBind_Dir = CNtlVector::UNIT_X;
	}
	else
	{
		if (false == (pNewbie->vBind_Dir).SafeNormalize())
		{
			_ASSERTE(0);
			return false;
		}
	}

	if (false == m_mapTableList.insert(std::pair<TBLIDX, sTBLDAT*>(pNewbie->tblidx, pNewbie)).second)
	{
		Table::CallErrorCallbackFunction(L"[File] : %s\r\n Table Tblidx[%u] is Duplicated ", m_wszXmlFileName, pNewbie->tblidx);
		_ASSERTE(0);
		return false;
	}

	//  [6/7/2006 john] : �߰� : race, class �� ���� tbldat ����
	if (false == SetNewbieTbldat(pNewbie->byRace, pNewbie->byClass, pNewbie))
	{
		_ASSERTE(0);
		m_mapTableList.erase(pNewbie->tblidx);
		return false;
	}


	return true;
}

bool NewbieTable::SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sNEWBIE_TBLDAT* pNewbie = (sNEWBIE_TBLDAT*)pvTable;

		if (0 == wcscmp(pstrDataName->c_str(), L"Tblidx"))
		{
			pNewbie->tblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Race"))
		{
			pNewbie->byRace = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Class"))
		{
			pNewbie->byClass = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"World_Id"))
		{
			pNewbie->world_Id = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Tutorial_World_Tblidx"))
		{
			pNewbie->tutorialWorld = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Spawn_Loc_X"))
		{
			pNewbie->vSpawn_Loc.x = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Spawn_Loc_Y"))
		{
			pNewbie->vSpawn_Loc.y = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Spawn_Loc_Z"))
		{
			pNewbie->vSpawn_Loc.z = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Spawn_Dir_X"))
		{
			pNewbie->vSpawn_Dir.x = READ_FLOAT(bstrData, pstrDataName->c_str(), 0.0f);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Spawn_Dir_Z"))
		{
			pNewbie->vSpawn_Dir.z = READ_FLOAT(bstrData, pstrDataName->c_str(), 0.0f);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Bind_Loc_X"))
		{
			pNewbie->vBind_Loc.x = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Bind_Loc_Y"))
		{
			pNewbie->vBind_Loc.y = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Bind_Loc_Z"))
		{
			pNewbie->vBind_Loc.z = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Bind_Dir_X"))
		{
			pNewbie->vBind_Dir.x = READ_FLOAT(bstrData, pstrDataName->c_str(), 0.0f);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Bind_Dir_Z"))
		{
			pNewbie->vBind_Dir.z = READ_FLOAT(bstrData, pstrDataName->c_str(), 0.0f);
		}
		else if (0 == wcsncmp(pstrDataName->c_str(), L"Item_Tblidx_", wcslen(L"Item_Tblidx_")))
		{
			bool bFound = false;

			WCHAR szBuffer[1024] = { 0x00, };
			for (int i = 0; i < NTL_MAX_NEWBIE_ITEM; i++)
			{
				swprintf(szBuffer, 1024, L"Item_Tblidx_%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					pNewbie->aitem_Tblidx[i] = READ_DWORD(bstrData);

					bFound = true;
					break;
				}
			}

			if (false == bFound)
			{
				Table::CallErrorCallbackFunction(L"[File] : %s\n[Error] : Unknown field name found!(Field Name = %s)", m_wszXmlFileName, pstrDataName->c_str());
				return false;
			}
		}
		else if (0 == wcsncmp(pstrDataName->c_str(), L"Position_", wcslen(L"Position_")))
		{
			bool bFound = false;

			WCHAR szBuffer[1024] = { 0x00, };
			for (int i = 0; i < NTL_MAX_NEWBIE_ITEM; i++)
			{
				swprintf(szBuffer, 1024, L"Position_%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					pNewbie->abyPos[i] = READ_BYTE(bstrData, pstrDataName->c_str(), EQUIP_SLOT_TYPE_UNKNOWN);

					bFound = true;
					break;
				}
			}

			if (false == bFound)
			{
				Table::CallErrorCallbackFunction(L"[File] : %s\n[Error] : Unknown field name found!(Field Name = %s)", m_wszXmlFileName, pstrDataName->c_str());
				return false;
			}
		}
		else if (0 == wcsncmp(pstrDataName->c_str(), L"Stack_Quantity_", wcslen(L"Stack_Quantity_")))
		{
			bool bFound = false;

			WCHAR szBuffer[1024] = { 0x00, };
			for (int i = 0; i < NTL_MAX_NEWBIE_ITEM; i++)
			{
				swprintf(szBuffer, 1024, L"Stack_Quantity_%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					pNewbie->abyStack_Quantity[i] = READ_BYTE(bstrData, pstrDataName->c_str(), 1);

					bFound = true;
					break;
				}
			}

			if (false == bFound)
			{
				Table::CallErrorCallbackFunction(L"[File] : %s\n[Error] : Unknown field name found!(Field Name = %s)", m_wszXmlFileName, pstrDataName->c_str());
				return false;
			}
		}
		else if (0 == wcsncmp(pstrDataName->c_str(), L"Skill_Tblidx_", wcslen(L"Skill_Tblidx_")))
		{
			bool bFound = false;

			WCHAR szBuffer[1024] = { 0x00, };
			for (int i = 0; i < NTL_MAX_NEWBIE_SKILL; i++)
			{
				swprintf(szBuffer, 1024, L"Skill_Tblidx_%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					pNewbie->aSkillTblidx[i] = READ_DWORD(bstrData);

					bFound = true;
					break;
				}
			}

			if (false == bFound)
			{
				Table::CallErrorCallbackFunction(L"[File] : %s\n[Error] : Unknown field name found!(Field Name = %s)", m_wszXmlFileName, pstrDataName->c_str());
				return false;
			}
		}
		else if (0 == wcsncmp(pstrDataName->c_str(), L"Quick_Tblidx", wcslen(L"Quick_Tblidx")))
		{
			bool bFound = false;

			WCHAR szBuffer[1024] = { 0x00, };
			for (int i = 0; i < NTL_MAX_NEWBIE_QUICKSLOT_COUNT; i++)
			{
				swprintf(szBuffer, 1024, L"Quick_Tblidx%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					pNewbie->asQuickData[i].tbilidx = READ_DWORD(bstrData);

					bFound = true;
					break;
				}
			}

			if (false == bFound)
			{
				Table::CallErrorCallbackFunction(L"[File] : %s\n[Error] : Unknown field name found!(Field Name = %s)", m_wszXmlFileName, pstrDataName->c_str());
				return false;
			}
		}
		else if (0 == wcsncmp(pstrDataName->c_str(), L"Quick_Type", wcslen(L"Quick_Type")))
		{
			bool bFound = false;

			WCHAR szBuffer[1024] = { 0x00, };
			for (int i = 0; i < NTL_MAX_NEWBIE_QUICKSLOT_COUNT; i++)
			{
				swprintf(szBuffer, 1024, L"Quick_Type%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					pNewbie->asQuickData[i].byType = READ_BYTE(bstrData, pstrDataName->c_str());

					bFound = true;
					break;
				}
			}

			if (false == bFound)
			{
				Table::CallErrorCallbackFunction(L"[File] : %s\n[Error] : Unknown field name found!(Field Name = %s)", m_wszXmlFileName, pstrDataName->c_str());
				return false;
			}
		}
		else if (0 == wcsncmp(pstrDataName->c_str(), L"Quick_Position", wcslen(L"Quick_Position")))
		{
			bool bFound = false;

			WCHAR szBuffer[1024] = { 0x00, };
			for (int i = 0; i < NTL_MAX_NEWBIE_QUICKSLOT_COUNT; i++)
			{
				swprintf(szBuffer, 1024, L"Quick_Position%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					pNewbie->asQuickData[i].byQuickSlot = READ_BYTE(bstrData, pstrDataName->c_str());

					bFound = true;
					break;
				}
			}

			if (false == bFound)
			{
				Table::CallErrorCallbackFunction(L"[File] : %s\n[Error] : Unknown field name found!(Field Name = %s)", m_wszXmlFileName, pstrDataName->c_str());
				return false;
			}
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Map_Name_Tblidx"))
		{
			pNewbie->mapNameTblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Default_Portal_Tblidx"))
		{
			pNewbie->defaultPortalTblidx = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"QItem_Tblidx_1"))
		{
			pNewbie->qItemTblidx1 = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"QPosition_1"))
		{
			pNewbie->byQPosition1 = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"QStack_Quantity_1"))
		{
			pNewbie->byQStackQuantity1 = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else
		{
			Table::CallErrorCallbackFunction(L"[File] : %s\n[Error] : Unknown field name found!(Field Name = %s)", m_wszXmlFileName, pstrDataName->c_str());
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}


sTBLDAT* NewbieTable::FindData(TBLIDX tblidx)
{
	if (0 == tblidx)
		return NULL;

	TABLEIT iter;
	iter = m_mapTableList.find(tblidx);
	if (End() == iter)
		return NULL;

	return (sTBLDAT*)(iter->second);
}

sTBLDAT * NewbieTable::GetNewbieTbldat(BYTE byRace, BYTE byClass)
{
	if (RACE_FIRST > byRace || RACE_LAST < byRace)
	{
		return NULL;
	}

	if (PC_CLASS_FIRST > byClass || PC_CLASS_LAST < byClass)
	{
		return NULL;
	}

	return m_aNewbieTbldat[byRace - RACE_FIRST][byClass - PC_CLASS_FIRST];
}

bool NewbieTable::SetNewbieTbldat(BYTE byRace, BYTE byClass, sTBLDAT * pTbldat)
{
	if (RACE_FIRST > byRace || RACE_LAST < byRace)
	{
		return false;
	}

	if (PC_CLASS_FIRST > byClass || PC_CLASS_LAST < byClass)
	{
		return false;
	}

	// already registered?
	if (NULL != m_aNewbieTbldat[byRace][byClass])
	{
		return false;
	}

	m_aNewbieTbldat[byRace][byClass] = pTbldat;

	return true;
}

bool NewbieTable::LoadFromBinary(Serializer& serializer, bool bReload)
{
	if (false == bReload)
	{
		Reset();
	}

	BYTE byMargin = 1;
	serializer >> byMargin;

	bool bLoop = true;
	do
	{
		sNEWBIE_TBLDAT* pTableData = new sNEWBIE_TBLDAT;
		if (NULL == pTableData)
		{
			//- yoshiki : To log system!
			Destroy();
			return false;
		}

		if (false == pTableData->LoadFromBinary(serializer))
		{
			delete pTableData;
			bLoop = false;
			break;
		}

		//  [4/26/2008 zeroera] : ���� : �����ϴ��� Load�� ���Ῡ�δ� File Loading���� �����Ѵ�
		if (false == AddTable(pTableData, bReload))
		{
			delete pTableData;
		}

	} while (false != bLoop);

	return true;
}

bool NewbieTable::SaveToBinary(Serializer& serializer)
{
	serializer.Refresh();

	BYTE byMargin = 1;
	serializer << byMargin;

	TABLE::iterator iter;
	for (iter = Begin(); End() != iter; iter++)
	{
		sNEWBIE_TBLDAT* pTableData = (sNEWBIE_TBLDAT*)(iter->second);

		pTableData->SaveToBinary(serializer);
	}

	return true;
}