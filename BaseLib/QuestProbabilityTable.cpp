#include "StdAfx.h"
#include "QuestProbabilityTable.h"
#include "Serializer.h"

WCHAR* QuestProbabilityTable::m_pwszSheetList[] =
{
	L"Table_Data_KOR",
	NULL
};

QuestProbabilityTable::QuestProbabilityTable(void)
{
	Init();
}

QuestProbabilityTable::~QuestProbabilityTable(void)
{
	Destroy();
}

bool QuestProbabilityTable::Create(DWORD dwCodePage)
{
	return Table::Create(dwCodePage);
}

void QuestProbabilityTable::Destroy()
{
	Table::Destroy();
}

void QuestProbabilityTable::Init()
{
}

void* QuestProbabilityTable::AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sQUEST_PROBABILITY_TBLDAT * pQuestProbability = new sQUEST_PROBABILITY_TBLDAT;
		if (NULL == pQuestProbability)
			return NULL;

		CPINFO cpInfo;
		if (false == GetCPInfo(dwCodePage, &cpInfo))
		{
			delete pQuestProbability;
			return NULL;
		}

		m_dwCodePage = dwCodePage;
		return pQuestProbability;
	}

	return NULL;
}

bool QuestProbabilityTable::DeallocNewTable(void* pvTable, WCHAR* pwszSheetName)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sQUEST_PROBABILITY_TBLDAT * pQuestProbability = (sQUEST_PROBABILITY_TBLDAT *)pvTable;
		if (FALSE != IsBadReadPtr(pQuestProbability, sizeof(*pQuestProbability)))
			return false;

		delete pQuestProbability;

		return true;
	}

	return false;
}

bool QuestProbabilityTable::AddTable(void * pvTable, bool bReload)
{
	UNREFERENCED_PARAMETER(bReload);

	sQUEST_PROBABILITY_TBLDAT * pTbldat = (sQUEST_PROBABILITY_TBLDAT *)pvTable;


	for (BYTE bySlot = 0; bySlot < NTL_QUEST_PROBABILITY_MAX_COUNT; bySlot++)
	{
		if (INVALID_TEMP_REWARD_TYPE == pTbldat->asProbabilityData[bySlot].byType)
		{
			break;
		}

		if (INVALID_TBLIDX == pTbldat->asProbabilityData[bySlot].tblidx
			&& INVALID_TBLIDX == pTbldat->asProbabilityData[bySlot].dwMaxValue
			&& INVALID_TBLIDX == pTbldat->asProbabilityData[bySlot].dwMinValue)
		{
			Table::CallErrorCallbackFunction(L"[File] : %s\r\n Table Tblidx[%u] reward type[%u] entity index[%u] : invalid value", m_wszXmlFileName, pTbldat->tblidx, pTbldat->asProbabilityData[bySlot].byType, bySlot);
			_ASSERTE(0);

			return false;
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

bool QuestProbabilityTable::SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sQUEST_PROBABILITY_TBLDAT * pProbabilityTbldat = (sQUEST_PROBABILITY_TBLDAT*)pvTable;

		if (0 == wcscmp(pstrDataName->c_str(), L"Tblidx"))
		{
			pProbabilityTbldat->tblidx = READ_TBLIDX(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Name"))
		{
			if (false == READ_STRINGW(bstrData, pProbabilityTbldat->wszName, _countof(pProbabilityTbldat->wszName)))
			{
				return false;
			}
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Note"))
		{
			if (false == READ_STRINGW(bstrData, pProbabilityTbldat->wszNote, _countof(pProbabilityTbldat->wszNote)))
			{
				return false;
			}
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Probability_Type"))
		{
			pProbabilityTbldat->byProbabilityType = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else
		{
			WCHAR szBuffer[1024] = { 0x00, };

			// Reward Type
			for (int i = 0; i < NTL_QUEST_PROBABILITY_MAX_COUNT; i++)
			{
				swprintf(szBuffer, 1024, L"Reward_Type%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					pProbabilityTbldat->asProbabilityData[i].byType = READ_BYTE(bstrData, pstrDataName->c_str());
					return true;
				}
			}

			// Reward Index
			for (int i = 0; i < NTL_QUEST_PROBABILITY_MAX_COUNT; i++)
			{
				swprintf(szBuffer, 1024, L"Reward_Tblidx%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					pProbabilityTbldat->asProbabilityData[i].tblidx = READ_TBLIDX(bstrData);
					return true;
				}
			}

			// Reward Index
			for (int i = 0; i < NTL_QUEST_PROBABILITY_MAX_COUNT; i++)
			{
				swprintf(szBuffer, 1024, L"Min_Value%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					pProbabilityTbldat->asProbabilityData[i].dwMinValue = READ_DWORD(bstrData);
					return true;
				}
			}

			// Reward Index
			for (int i = 0; i < NTL_QUEST_PROBABILITY_MAX_COUNT; i++)
			{
				swprintf(szBuffer, 1024, L"Max_Value%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					pProbabilityTbldat->asProbabilityData[i].dwMaxValue = READ_DWORD(bstrData);
					return true;
				}
			}

			// Rate
			for (int i = 0; i < NTL_QUEST_PROBABILITY_MAX_COUNT; i++)
			{
				swprintf(szBuffer, 1024, L"Drop_Rate%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					pProbabilityTbldat->asProbabilityData[i].fRate = READ_FLOAT(bstrData, pstrDataName->c_str());
					return true;
				}
			}

			Table::CallErrorCallbackFunction(L"[File] : %s\n[Error] : Unknown field name found!(Field Name = %s)", m_wszXmlFileName, pstrDataName->c_str());
			return false;

		} // end if
	} // end if
	else
	{
		return false;
	}

	return true;
}


sTBLDAT* QuestProbabilityTable::FindData(TBLIDX tblidx)
{
	if (0 == tblidx)
		return NULL;

	TABLEIT iter;
	iter = m_mapTableList.find(tblidx);
	if (End() == iter)
		return NULL;

	return (sTBLDAT*)(iter->second);
}

bool QuestProbabilityTable::LoadFromBinary(Serializer& serializer, bool bReload)
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
		sQUEST_PROBABILITY_TBLDAT* pTableData = new sQUEST_PROBABILITY_TBLDAT;
		if (NULL == pTableData)
		{
			// Log : [11/29/2007 niam]

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

bool QuestProbabilityTable::SaveToBinary(Serializer& serializer)
{
	serializer.Refresh();

	BYTE byMargin = 1;
	serializer << byMargin;

	TABLE::iterator iter;
	for (iter = Begin(); End() != iter; iter++)
	{
		sQUEST_PROBABILITY_TBLDAT* pTableData = (sQUEST_PROBABILITY_TBLDAT*)(iter->second);

		pTableData->SaveToBinary(serializer);
	}

	return true;
}
