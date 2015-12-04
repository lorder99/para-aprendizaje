#include "StdAfx.h"
#include "PortalTable.h"
#include "Serializer.h"


WCHAR* PortalTable::m_pwszSheetList[] =
{
	L"Table_Data_KOR",
	NULL
};

PortalTable::PortalTable(void)
{
	Init();
}

PortalTable::~PortalTable(void)
{
	Destroy();
}

bool PortalTable::Create(DWORD dwCodePage)
{
	return Table::Create(dwCodePage);
}

void PortalTable::Destroy()
{
	Table::Destroy();
}

void PortalTable::Init()
{
}

void* PortalTable::AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sPORTAL_TBLDAT* pPortal = new sPORTAL_TBLDAT;
		if (NULL == pPortal)
			return NULL;

		CPINFO cpInfo;
		if (false == GetCPInfo(dwCodePage, &cpInfo))
		{
			delete pPortal;
			return NULL;
		}

		m_dwCodePage = dwCodePage;
		return pPortal;
	}

	return NULL;
}

bool PortalTable::DeallocNewTable(void* pvTable, WCHAR* pwszSheetName)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sPORTAL_TBLDAT* pPortal = (sPORTAL_TBLDAT*)pvTable;
		if (FALSE != IsBadReadPtr(pPortal, sizeof(*pPortal)))
			return false;

		delete pPortal;

		return true;
	}

	return false;
}

bool PortalTable::AddTable(void * pvTable, bool bReload)
{
	UNREFERENCED_PARAMETER(bReload);

	sPORTAL_TBLDAT* pTbldat = (sPORTAL_TBLDAT*)pvTable;

	if (false == m_mapTableList.insert(std::pair<TBLIDX, sTBLDAT*>(pTbldat->tblidx, pTbldat)).second)
	{
		Table::CallErrorCallbackFunction(L"[File] : %s\r\n Table Tblidx[%u] is Duplicated ", m_wszXmlFileName, pTbldat->tblidx);
		_ASSERTE(0);
		return false;
	}


	return true;
}

bool PortalTable::SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sPORTAL_TBLDAT* pPortal = (sPORTAL_TBLDAT*)pvTable;

		if (0 == wcscmp(pstrDataName->c_str(), L"Tblidx"))
		{
			pPortal->tblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Point_Name"))
		{
			pPortal->dwPointName = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Point_Name_Text"))
		{
			READ_STRING(bstrData, pPortal->szPointNameText, _countof(pPortal->szPointNameText));
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"World"))
		{
			pPortal->worldId = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Loc_X"))
		{
			pPortal->vLoc.x = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Loc_Y"))
		{
			pPortal->vLoc.y = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Loc_Z"))
		{
			pPortal->vLoc.z = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Dir_X"))
		{
			pPortal->vDir.x = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Dir_Z"))
		{
			pPortal->vDir.z = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Map_X"))
		{
			pPortal->vMap.x = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Map_Y"))
		{
			pPortal->vMap.z = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcsncmp(pstrDataName->c_str(), L"Point_", wcslen(L"Point_")))
		{
			bool bFound = false;

			WCHAR szBuffer[1024] = { 0x00, };
			for (int i = 0; i < DBO_MAX_POINT_PORTAL; i++)
			{
				swprintf(szBuffer, 1024, L"Point_%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					pPortal->aPoint[i] = READ_BYTE(bstrData, pstrDataName->c_str());

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
		else if (0 == wcsncmp(pstrDataName->c_str(), L"Zeny_", wcslen(L"Zeny_")))
		{
			bool bFound = false;

			WCHAR szBuffer[1024] = { 0x00, };
			for (int i = 0; i < DBO_MAX_POINT_PORTAL; i++)
			{
				swprintf(szBuffer, 1024, L"Zeny_%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					pPortal->adwPointZenny[i] = READ_DWORD(bstrData);

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


sTBLDAT* PortalTable::FindData(TBLIDX tblidx)
{
	if (0 == tblidx)
		return NULL;

	TABLEIT iter;
	iter = m_mapTableList.find(tblidx);
	if (End() == iter)
		return NULL;

	return (sTBLDAT*)(iter->second);
}

bool PortalTable::LoadFromBinary(Serializer& serializer, bool bReload)
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
		sPORTAL_TBLDAT* pTableData = new sPORTAL_TBLDAT;
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

bool PortalTable::SaveToBinary(Serializer& serializer)
{
	serializer.Refresh();

	BYTE byMargin = 1;
	serializer << byMargin;

	TABLE::iterator iter;
	for (iter = Begin(); End() != iter; iter++)
	{
		sPORTAL_TBLDAT* pTableData = (sPORTAL_TBLDAT*)(iter->second);

		pTableData->SaveToBinary(serializer);
	}

	return true;
}