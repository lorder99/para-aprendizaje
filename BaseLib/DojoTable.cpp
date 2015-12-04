#include "stdafx.h"
#include "DojoTable.h"
#include "Serializer.h"

WCHAR* DojoTable::m_pwszSheetList[] =
{
	L"Table_Data_KOR",
	NULL
};

DojoTable::DojoTable(void)
{
	Init();
}

DojoTable::~DojoTable(void)
{
	Destroy();
}

void DojoTable::Init(void)
{
}

bool DojoTable::Create(DWORD dwCodePage)
{
	return Table::Create(dwCodePage);
}

void DojoTable::Destroy(void)
{
	Table::Destroy();
}

void* DojoTable::AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sDOJO_TBLDAT* pNewObj = new sDOJO_TBLDAT;
		if (NULL == pNewObj)
		{
			return NULL;
		}

		CPINFO cpInfo;
		if (!GetCPInfo(dwCodePage, &cpInfo))
		{
			delete pNewObj;
			return NULL;
		}

		m_dwCodePage = dwCodePage;
		return pNewObj;
	}

	return NULL;
}

bool DojoTable::DeallocNewTable(void* pvTable, WCHAR* pwszSheetName)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sDOJO_TBLDAT* pObj = (sDOJO_TBLDAT*)pvTable;
		if (IsBadReadPtr(pObj, sizeof(*pObj))) return false;

		delete pObj;

		return true;
	}

	return false;
}

bool DojoTable::AddTable(void * pvTable, bool bReload)
{
	UNREFERENCED_PARAMETER(bReload);

	sDOJO_TBLDAT * pTbldat = (sDOJO_TBLDAT*)pvTable;

	if (false == m_mapTableList.insert(std::pair<TBLIDX, sTBLDAT*>(pTbldat->tblidx, pTbldat)).second)
	{
		Table::CallErrorCallbackFunction(L"[File] : %s\r\n Table Tblidx[%u] is Duplicated ", m_wszXmlFileName, pTbldat->tblidx);
		_ASSERTE(0);
		return false;
	}


	return true;
}

bool DojoTable::SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData)
{
	static char szTemp[1024] = { 0x00, };

	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sDOJO_TBLDAT * pTbldat = (sDOJO_TBLDAT*)pvTable;

		if (0 == wcscmp(pstrDataName->c_str(), L"Tblidx"))
		{
			pTbldat->tblidx = READ_TBLIDX(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Zone_Tblidx"))
		{
			pTbldat->zoneTblidx = READ_TBLIDX(bstrData);
		}
		else if (0 == wcsncmp(pstrDataName->c_str(), L"Object_Tblidx_", wcslen(L"Object_Tblidx_")))
		{
			bool bFound = false;

			WCHAR szBuffer[1024] = { 0x00, };
			for (int i = 0; i < DOJO_MAX_UPGRADE_OBJECT_COUNT; i++)
			{
				swprintf(szBuffer, 1024, L"Object_Tblidx_%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					pTbldat->objectTblidx[i] = READ_DWORD(bstrData);

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
		else if (0 == wcscmp(pstrDataName->c_str(), L"Map_Name"))
		{
			pTbldat->mapName = READ_TBLIDX(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Receive_Hour"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pTbldat->byReceiveHour = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Receive_Minute"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pTbldat->byReceiveMinute = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Repeat_Type"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pTbldat->byRepeatType = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Repeat_Time"))
		{
			pTbldat->byRepeatTime = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Week_Bit_Flag"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pTbldat->wWeekBitFlag = (WORD)READ_BITFLAG(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Receive_Duration"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pTbldat->byReceiveDuration = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Reject_Duration"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pTbldat->byRejectDuration = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Standby_Duration"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pTbldat->byStandbyDuration = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Initial_Duration"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pTbldat->byInitialDuration = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Ready_Duration"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pTbldat->byReadyDuration = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Battle_Duration"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pTbldat->byBattleDuration = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Receive_Point"))
		{
			pTbldat->dwReceivePoint = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Receive_Zeny"))
		{
			pTbldat->dwReceiveZenny = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Controller_Tblidx"))
		{
			pTbldat->controllerTblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Battle_Point_Goal"))
		{
			pTbldat->dwBattlePointGoal = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Battle_Point_Get"))
		{
			pTbldat->dwBattlePointGet = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Battle_Point_Charge"))
		{
			pTbldat->dwBattlePointCharge = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Charge_Point_Goal"))
		{
			pTbldat->dwChargePointGoal = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Charge_Time"))
		{
			pTbldat->dwChargeTime = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Charge_Time_Point"))
		{
			pTbldat->dwChageTimePoint = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Rock_Tblidx"))
		{
			pTbldat->rockTblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcsncmp(pstrDataName->c_str(), L"Get_Point", wcslen(L"Get_Point")))
		{
			bool bFound = false;

			WCHAR szBuffer[1024] = { 0x00, };
			for (int i = 0; i < DOJO_MAX_REWARD_TYPE_COUNT; i++)
			{
				swprintf(szBuffer, 1024, L"Get_Point%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					pTbldat->asRawrd[i].dwGetPoint = READ_DWORD(bstrData);

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
		else if (0 == wcsncmp(pstrDataName->c_str(), L"Get_Rock", wcslen(L"Get_Rock")))
		{
			bool bFound = false;

			WCHAR szBuffer[1024] = { 0x00, };
			for (int i = 0; i < DOJO_MAX_REWARD_TYPE_COUNT; i++)
			{
				swprintf(szBuffer, 1024, L"Get_Rock%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					pTbldat->asRawrd[i].byGetRock = READ_BYTE(bstrData, pstrDataName->c_str());

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

sTBLDAT* DojoTable::FindData(TBLIDX tblidx)
{
	TABLEIT iter = m_mapTableList.find(tblidx);
	if (m_mapTableList.end() == iter)
	{
		return NULL;
	}

	return (sTBLDAT*)(iter->second);
}

sTBLDAT* DojoTable::GetDojoData()
{
	TABLEIT iter = m_mapTableList.begin();
	if (m_mapTableList.end() == iter)
	{
		return NULL;
	}

	return (sTBLDAT*)(iter->second);
}

bool DojoTable::IsWorldDojo(TBLIDX worldIdx)
{
	TABLE::iterator iter;
	for (iter = Begin(); End() != iter; iter++)
	{
		sDOJO_TBLDAT* pTableData = (sDOJO_TBLDAT*)(iter->second);

		if (worldIdx == pTableData->zoneTblidx)
		{
			return true;
		}
	}
	return false;
}

TBLIDX DojoTable::GetTblidxByWorldIdx(TBLIDX worldIdx)
{
	TABLE::iterator iter;
	for (iter = Begin(); End() != iter; iter++)
	{
		sDOJO_TBLDAT* pTableData = (sDOJO_TBLDAT*)(iter->second);

		if (worldIdx == pTableData->zoneTblidx)
		{
			return pTableData->tblidx;
		}
	}
	return INVALID_TBLIDX;
}

bool DojoTable::LoadFromBinary(Serializer& serializer, bool bReload)
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
		sDOJO_TBLDAT* pTableData = new sDOJO_TBLDAT;
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

bool DojoTable::SaveToBinary(Serializer& serializer)
{
	serializer.Refresh();

	BYTE byMargin = 1;
	serializer << byMargin;

	TABLE::iterator iter;
	for (iter = Begin(); End() != iter; iter++)
	{
		sDOJO_TBLDAT* pTableData = (sDOJO_TBLDAT*)(iter->second);

		pTableData->SaveToBinary(serializer);
	}

	return true;
}
