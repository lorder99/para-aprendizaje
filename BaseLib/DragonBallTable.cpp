#include "StdAfx.h"
#include "DragonBallTable.h"
#include "Serializer.h"


WCHAR* DragonBallTable::m_pwszSheetList[] =
{
	L"Table_Data_KOR",
	NULL
};

DragonBallTable::DragonBallTable(void)
{
	Init();
}

DragonBallTable::~DragonBallTable(void)
{
	Destroy();
}

bool DragonBallTable::Create(DWORD dwCodePage)
{
	::ZeroMemory(m_aDBTbldat, sizeof(m_aDBTbldat));
	m_dragonGroup.clear();
	return Table::Create(dwCodePage);
}

void DragonBallTable::Destroy()
{
	::ZeroMemory(m_aDBTbldat, sizeof(m_aDBTbldat));
	m_dragonGroup.clear();
	Table::Destroy();
}

void DragonBallTable::Init()
{
	m_dragonGroup.clear();
}

void* DragonBallTable::AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sDRAGONBALL_TBLDAT* pNewDragonBall = new sDRAGONBALL_TBLDAT;
		if (NULL == pNewDragonBall)
			return NULL;

		CPINFO cpInfo;
		if (false == GetCPInfo(dwCodePage, &cpInfo))
		{
			delete pNewDragonBall;
			return NULL;
		}

		m_dwCodePage = dwCodePage;
		return pNewDragonBall;
	}

	return NULL;
}

bool DragonBallTable::DeallocNewTable(void* pvTable, WCHAR* pwszSheetName)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sDRAGONBALL_TBLDAT* pDragonBall = (sDRAGONBALL_TBLDAT*)pvTable;
		if (FALSE != IsBadReadPtr(pDragonBall, sizeof(*pDragonBall)))
			return false;

		delete pDragonBall;

		return true;
	}

	return false;
}

eDRAGON_BALL_TYPE DragonBallTable::GetDropItemType(UINT dropTblidx)
{
	sDRAGONBALL_TBLDAT* pTblData = NULL;
	for (BYTE j = SCHEDULE_EVENT_TYPE_FIRST; j <= SCHEDULE_EVENT_TYPE_LAST; ++j)
	{
		pTblData = (sDRAGONBALL_TBLDAT*)GetDBTbldat(j);
		if (pTblData && pTblData->ballDropTblidx == dropTblidx)
		{
			return (eDRAGON_BALL_TYPE)pTblData->byBallType;
		}
	}

	return DRAGON_BALL_TYPE_NONE;
}

bool DragonBallTable::AddTable(void * pvTable, bool bReload)
{
	UNREFERENCED_PARAMETER(bReload);

	sDRAGONBALL_TBLDAT* pTbldat = (sDRAGONBALL_TBLDAT*)pvTable;

	if (false == m_mapTableList.insert(std::pair<TBLIDX, sTBLDAT*>(pTbldat->tblidx, pTbldat)).second)
	{
		Table::CallErrorCallbackFunction(L"[File] : %s\r\n Table Tblidx[%u] is Duplicated ", m_wszXmlFileName, pTbldat->tblidx);
		_ASSERTE(0);
		return false;
	}

	if (false == SetDBTbldatByGroup(pTbldat->dwAltarGroup, pTbldat->tblidx))
	{
		_ASSERTE(0);
		m_mapTableList.erase(pTbldat->tblidx);
		return false;
	}

	if (false == SetDBTbldat(pTbldat->byBallType, pTbldat))
	{
		_ASSERTE(0);
		m_mapTableList.erase(pTbldat->tblidx);
		return false;
	}
	return true;
}

bool DragonBallTable::SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sDRAGONBALL_TBLDAT* pDragonBall = (sDRAGONBALL_TBLDAT*)pvTable;

		if (0 == wcscmp(pstrDataName->c_str(), L"Tblidx"))
		{
			pDragonBall->tblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Altar_Group"))
		{
			pDragonBall->dwAltarGroup = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Ball_Type"))
		{
			pDragonBall->byBallType = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Ball_1_Tblidx"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pDragonBall->aBallTblidx[0] = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Ball_2_Tblidx"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pDragonBall->aBallTblidx[1] = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Ball_3_Tblidx"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pDragonBall->aBallTblidx[2] = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Ball_4_Tblidx"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pDragonBall->aBallTblidx[3] = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Ball_5_Tblidx"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pDragonBall->aBallTblidx[4] = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Ball_6_Tblidx"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pDragonBall->aBallTblidx[5] = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Ball_7_Tblidx"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pDragonBall->aBallTblidx[6] = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Ball_Drop_Tblidx"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pDragonBall->ballDropTblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Ball_Junk_Tblidx"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pDragonBall->ballJunkTblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Start_Dialog"))
		{
			pDragonBall->startDialog = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"End_Dialog"))
		{
			pDragonBall->endDialog = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Hurry_Dialog"))
		{
			pDragonBall->hurryDialog = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Timeover_Dialog"))
		{
			pDragonBall->timeoverDialog = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"No_Repeat_Dialog"))
		{
			pDragonBall->noRepeatDialog = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Dragon_NPC_Tblidx"))
		{
			pDragonBall->dragonNPCTblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Default_Summon_Chat"))
		{
			pDragonBall->defaultSummonChat = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Inventory_Full_Dialog"))
		{
			pDragonBall->inventoryFullDialog = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Appear_Dir_X"))
		{
			pDragonBall->fDir.x = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Appear_Dir_Z"))
		{
			pDragonBall->fDir.z = READ_FLOAT(bstrData, pstrDataName->c_str());
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


sTBLDAT* DragonBallTable::FindData(TBLIDX tblidx)
{
	if (0 == tblidx)
		return NULL;

	TABLEIT iter;
	iter = m_mapTableList.find(tblidx);
	if (End() == iter)
		return NULL;

	return (sTBLDAT*)(iter->second);
}

bool DragonBallTable::LoadFromBinary(Serializer& serializer, bool bReload)
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
		sDRAGONBALL_TBLDAT* pTableData = new sDRAGONBALL_TBLDAT;
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

bool DragonBallTable::SaveToBinary(Serializer& serializer)
{
	serializer.Refresh();

	BYTE byMargin = 1;
	serializer << byMargin;

	TABLE::iterator iter;
	for (iter = Begin(); End() != iter; iter++)
	{
		sDRAGONBALL_TBLDAT* pTableData = (sDRAGONBALL_TBLDAT*)(iter->second);

		pTableData->SaveToBinary(serializer);
	}

	return true;
}

sTBLDAT * DragonBallTable::GetDBTbldatByGroup(DWORD dwGrouID)
{
	if (0 == dwGrouID)
		return NULL;

	DRAGONBALLGROUP_IT iter = m_dragonGroup.find(dwGrouID);
	if (m_dragonGroup.end() == iter)
		return NULL;

	sTBLDAT* psTable = FindData(iter->second);
	return psTable;

}

bool DragonBallTable::SetDBTbldatByGroup(DWORD dwGroupID, TBLIDX tblidx)
{
	if (0 == dwGroupID)
	{
		return false;
	}

	if (NULL == FindData(tblidx))
	{
		return false;
	}

	DRAGONBALLGROUP_IT it = m_dragonGroup.insert(DRAGONBALLGROUP_VAL(dwGroupID, tblidx));
	if (it == m_dragonGroup.end())
	{
		_ASSERTE(0);
		return false;
	}

	return true;
}

sTBLDAT * DragonBallTable::GetDBTbldat(BYTE byType)
{
	if (byType > SCHEDULE_EVENT_TYPE_LAST)
	{
		return NULL;
	}

	return m_aDBTbldat[byType];
}

bool DragonBallTable::SetDBTbldat(BYTE byType, sTBLDAT * pTbldat)
{
	if (byType > SCHEDULE_EVENT_TYPE_LAST)
	{
		return NULL;
	}

	// already registered?
	if (NULL != m_aDBTbldat[byType])
	{
		return true;
	}

	m_aDBTbldat[byType] = pTbldat;

	return true;
}

bool DragonBallTable::GetType(UINT uiSerial, OUT BYTE& byType, OUT BYTE& byNumber)
{
	sDRAGONBALL_TBLDAT* pTblData = NULL;

	for (BYTE j = SCHEDULE_EVENT_TYPE_FIRST; j <= SCHEDULE_EVENT_TYPE_LAST; ++j)
	{
		pTblData = (sDRAGONBALL_TBLDAT*)GetDBTbldat(j);
		if (pTblData)
		{
			for (BYTE i = 0; i < NTL_ITEM_MAX_DRAGONBALL; ++i)
			{
				if (pTblData->aBallTblidx[i] == uiSerial)
				{
					byType = j;
					byNumber = i + 1;
					return true;
				}
			}
		}
	}

	return false;
}