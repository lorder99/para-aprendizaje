#include "StdAfx.h"
#include "WorldMapTable.h"
#include "Serializer.h"

WCHAR* WorldMapTable::m_pwszSheetList[] =
{
	L"Table_Data_KOR",
	NULL
};

WorldMapTable::WorldMapTable(void)
{
	Init();
}

WorldMapTable::~WorldMapTable(void)
{
	Destroy();
}

bool WorldMapTable::Create(DWORD dwCodePage)
{
	return Table::Create(dwCodePage);
}

void WorldMapTable::Destroy()
{
	Table::Destroy();
}

void WorldMapTable::Init()
{
}

void* WorldMapTable::AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sWORLD_MAP_TBLDAT* pWorld = new sWORLD_MAP_TBLDAT;
		if (NULL == pWorld)
			return NULL;

		CPINFO cpInfo;
		if (false == GetCPInfo(dwCodePage, &cpInfo))
		{
			delete pWorld;
			return NULL;
		}

		m_dwCodePage = dwCodePage;
		return pWorld;
	}

	return NULL;
}

bool WorldMapTable::DeallocNewTable(void* pvTable, WCHAR* pwszSheetName)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sWORLD_MAP_TBLDAT* pWorld = (sWORLD_MAP_TBLDAT*)pvTable;
		if (FALSE != IsBadReadPtr(pWorld, sizeof(*pWorld)))
			return false;

		delete pWorld;

		return true;
	}

	return false;
}

bool WorldMapTable::AddTable(void * pvTable, bool bReload)
{
	sWORLD_MAP_TBLDAT* pTbldat = (sWORLD_MAP_TBLDAT*)pvTable;
	sWORLD_MAP_TBLDAT * pExistTbldat = NULL;

	if (bReload)
	{
		pExistTbldat = (sWORLD_MAP_TBLDAT*)FindData(pTbldat->tblidx);
		if (pExistTbldat)
		{
			CopyMemory(pTbldat, pExistTbldat, pTbldat->GetDataSize());
			// ����Ÿ�� ������ ���� false ��ȯ
			return true;
		}
	}

	if (false == m_mapTableList.insert(std::pair<TBLIDX, sTBLDAT*>(pTbldat->tblidx, pTbldat)).second)
	{
		Table::CallErrorCallbackFunction(L"[File] : %s\r\n Table Tblidx[%u] is Duplicated ", m_wszXmlFileName, pTbldat->tblidx);
		_ASSERTE(0);
		return false;
	}


	return true;
}

bool WorldMapTable::SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sWORLD_MAP_TBLDAT* pWorld = (sWORLD_MAP_TBLDAT*)pvTable;

		if (0 == wcscmp(pstrDataName->c_str(), L"Tblidx"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pWorld->tblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"World_Tblidx"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pWorld->World_Tblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Zone_Tblidx"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pWorld->Zone_Tblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Worldmap_Name"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pWorld->Worldmap_Name = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Name_Text"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			READ_STRINGW(bstrData, pWorld->wszNameText, _countof(pWorld->wszNameText));
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Validity_Able"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pWorld->bValidityAble = READ_BOOL(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Map_Type"))
		{
			pWorld->byMapType = READ_BYTE(bstrData, pstrDataName->c_str(), 0);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Standard_Loc_X"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pWorld->vStandardLoc.x = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Standard_Loc_Z"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pWorld->vStandardLoc.z = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Worldmap_Scale"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pWorld->fWorldmapScale = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcsncmp(pstrDataName->c_str(), L"Warfog_", wcslen(L"Warfog_")))
		{
			bool bFound = false;
			WCHAR szBuffer[1024] = { 0x00, };

			for (int i = 0; i < DBO_WORLD_MAP_TABLE_COUNT_WORLD_WARFOG; i++)
			{
				swprintf(szBuffer, 1024, L"Warfog_%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					pWorld->wWarfog[i] = READ_WORD(bstrData, pstrDataName->c_str());
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


sTBLDAT* WorldMapTable::FindData(TBLIDX tblidx)
{
	if (0 == tblidx)
		return NULL;

	TABLEIT iter;
	iter = m_mapTableList.find(tblidx);
	if (End() == iter)
		return NULL;

	return (sTBLDAT*)(iter->second);
}

bool WorldMapTable::LoadFromBinary(Serializer& serializer, bool bReload)
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
		sWORLD_MAP_TBLDAT* pTableData = new sWORLD_MAP_TBLDAT;
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

bool WorldMapTable::SaveToBinary(Serializer& serializer)
{
	serializer.Refresh();

	BYTE byMargin = 1;
	serializer << byMargin;

	TABLE::iterator iter;
	for (iter = Begin(); End() != iter; iter++)
	{
		sWORLD_MAP_TBLDAT* pTableData = (sWORLD_MAP_TBLDAT*)(iter->second);

		pTableData->SaveToBinary(serializer);
	}

	return true;
}