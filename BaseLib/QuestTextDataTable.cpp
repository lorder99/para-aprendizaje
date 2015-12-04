#include "StdAfx.h"
#include "QuestTextDataTable.h"
#include "Serializer.h"

WCHAR* QuestTextDataTable::m_pwszSheetList[] =
{
	L"Table_Data_KOR",
	NULL
};

QuestTextDataTable::QuestTextDataTable(void)
{
	Init();
}

QuestTextDataTable::~QuestTextDataTable(void)
{
	Destroy();
}

bool QuestTextDataTable::Create(DWORD dwCodePage)
{
	return Table::Create(dwCodePage);
}

void QuestTextDataTable::Destroy()
{
	Table::Destroy();
}

void QuestTextDataTable::Init()
{
}

void* QuestTextDataTable::AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sQUEST_TEXT_DATA_TBLDAT* pQuestTextData = new sQUEST_TEXT_DATA_TBLDAT;
		if (NULL == pQuestTextData)
			return NULL;

		CPINFO cpInfo;
		if (false == GetCPInfo(dwCodePage, &cpInfo))
		{
			delete pQuestTextData;
			return NULL;
		}

		m_dwCodePage = dwCodePage;
		return pQuestTextData;
	}

	return NULL;
}

bool QuestTextDataTable::DeallocNewTable(void* pvTable, WCHAR* pwszSheetName)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sQUEST_TEXT_DATA_TBLDAT* pQuestTextData = (sQUEST_TEXT_DATA_TBLDAT*)pvTable;
		if (FALSE != IsBadReadPtr(pQuestTextData, sizeof(*pQuestTextData)))
			return false;

		delete pQuestTextData;

		return true;
	}

	return false;
}

bool QuestTextDataTable::AddTable(void * pvTable, bool bReload)
{
	UNREFERENCED_PARAMETER(bReload);

	sQUEST_TEXT_DATA_TBLDAT* pTbldat = (sQUEST_TEXT_DATA_TBLDAT*)pvTable;

	if (false == m_mapTableList.insert(std::pair<TBLIDX, sTBLDAT*>(pTbldat->tblidx, pTbldat)).second)
	{
		Table::CallErrorCallbackFunction(L"[File] : %s\r\n Table Tblidx[%u] is Duplicated ", m_wszXmlFileName, pTbldat->tblidx);
		_ASSERTE(0);
		return false;
	}


	return true;
}

bool QuestTextDataTable::SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sQUEST_TEXT_DATA_TBLDAT* pQuestTextData = (sQUEST_TEXT_DATA_TBLDAT*)pvTable;

		if (0 == wcscmp(pstrDataName->c_str(), L"Quest_Text_Index"))
		{
			pQuestTextData->tblidx = READ_TBLIDX(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Quest_Text"))
		{
			if (false == READ_STR(pQuestTextData->wstrText, bstrData))
			{
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


sTBLDAT* QuestTextDataTable::FindData(TBLIDX tblidx)
{
	if (0 == tblidx)
		return NULL;

	TABLEIT iter;
	iter = m_mapTableList.find(tblidx);
	if (End() == iter)
		return NULL;

	return (sTBLDAT*)(iter->second);
}

void QuestTextDataTable::FindDataRange(TBLIDX tblBegin, TBLIDX tblEnd, std::vector<sQUEST_TEXT_DATA_TBLDAT*>& vecRangeList)
{
	TABLEIT iter = m_mapTableList.begin();
	for (; iter != m_mapTableList.end(); ++iter)
	{
		TBLIDX tblIdx = iter->first;

		if (tblIdx < tblBegin)
			continue;

		if (tblIdx > tblEnd)
			break;

		vecRangeList.push_back((sQUEST_TEXT_DATA_TBLDAT*)iter->second);
	}
}

bool QuestTextDataTable::LoadFromBinary(Serializer& serializer, bool bReload)
{
	if (false == bReload)
	{
		Reset();
	}

	BYTE byMargin = 1;
	serializer >> byMargin;

	while (0 < serializer.GetDataSize())
	{
		sQUEST_TEXT_DATA_TBLDAT* pTableData = new sQUEST_TEXT_DATA_TBLDAT;
		if (NULL == pTableData)
		{
			//- yoshiki : To log system!
			Destroy();
			return false;
		}

		WORD wTextLength = 0;
		if (serializer.GetDataSize() < sizeof(pTableData->tblidx) + sizeof(wTextLength))
		{
			//- yoshiki : To log system!

			delete pTableData;

			Destroy();
			return false;
		}
		serializer >> pTableData->tblidx;
		serializer >> wTextLength;

		if (serializer.GetDataSize() < (int)(wTextLength * sizeof(WCHAR)))
		{
			//- yoshiki : To log system!

			delete pTableData;

			Destroy();
			return false;
		}

		WCHAR* pwszText = new WCHAR[wTextLength + 1];
		if (NULL == pwszText)
		{
			//- yoshiki : To log system!

			delete pTableData;

			Destroy();
			return false;
		}
		serializer.Out(pwszText, wTextLength * sizeof(WCHAR));
		pwszText[wTextLength] = L'\0';

		pTableData->wstrText = pwszText;

		delete[] pwszText;

		//  [4/26/2008 zeroera] : ���� : �����ϴ��� Load�� ���Ῡ�δ� File Loading���� �����Ѵ�
		if (false == AddTable(pTableData, bReload))
		{
			delete pTableData;
		}
	};

	return true;
}

bool QuestTextDataTable::SaveToBinary(Serializer& serializer)
{
	serializer.Refresh();

	BYTE byMargin = 1;
	serializer << byMargin;

	TABLE::iterator iter;
	for (iter = Begin(); End() != iter; iter++)
	{
		sQUEST_TEXT_DATA_TBLDAT* pTableData = (sQUEST_TEXT_DATA_TBLDAT*)(iter->second);

		serializer << pTableData->tblidx;
		serializer << (WORD)((pTableData->wstrText).size());
		serializer.In((pTableData->wstrText).c_str(), (int)((pTableData->wstrText).size() * sizeof(WCHAR)));
	}

	return true;
}