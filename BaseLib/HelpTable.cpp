#include "StdAfx.h"
#include "HelpTable.h"
#include "Serializer.h"

WCHAR* HelpTable::m_pwszSheetList[] =
{
	L"Table_Data_KOR",
	NULL
};

HelpTable::HelpTable(void)
{
	Init();
}

HelpTable::~HelpTable(void)
{
	Destroy();
}

bool HelpTable::Create(DWORD dwCodePage)
{
	return Table::Create(dwCodePage);
}

void HelpTable::Destroy()
{
	Table::Destroy();
}

void HelpTable::Init()
{
}

void* HelpTable::AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sHELP_TBLDAT* pNewHelp = new sHELP_TBLDAT;
		if (NULL == pNewHelp)
			return NULL;

		CPINFO cpInfo;
		if (false == GetCPInfo(dwCodePage, &cpInfo))
		{
			delete pNewHelp;
			return NULL;
		}

		m_dwCodePage = dwCodePage;
		return pNewHelp;
	}

	return NULL;
}

bool HelpTable::DeallocNewTable(void* pvTable, WCHAR* pwszSheetName)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sHELP_TBLDAT* pHelp = (sHELP_TBLDAT*)pvTable;
		if (FALSE != IsBadReadPtr(pHelp, sizeof(*pHelp)))
			return false;

		delete pHelp;

		return true;
	}

	return false;
}

bool HelpTable::AddTable(void * pvTable, bool bReload)
{
	UNREFERENCED_PARAMETER(bReload);

	sHELP_TBLDAT* pTbldat = (sHELP_TBLDAT*)pvTable;

	if (false == m_mapTableList.insert(std::pair<TBLIDX, sTBLDAT*>(pTbldat->tblidx, pTbldat)).second)
	{
		Table::CallErrorCallbackFunction(L"[File] : %s\r\n Table Tblidx[%u] is Duplicated ", m_wszXmlFileName, pTbldat->tblidx);
		_ASSERTE(0);
		return false;
	}


	return true;
}

bool HelpTable::SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sHELP_TBLDAT* pHelp = (sHELP_TBLDAT*)pvTable;

		if (0 == wcscmp(pstrDataName->c_str(), L"Help_Tblidx"))
		{
			pHelp->tblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Category"))
		{
			pHelp->byCategory = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Help_Title"))
		{
			pHelp->dwHelpTitle = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Popo_Hint"))
		{
			pHelp->dwPopoHint = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Help_HTML_Name"))
		{
			READ_STRINGW(bstrData, pHelp->wszHelpHTMLName, _countof(pHelp->wszHelpHTMLName));
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Condition_Check"))
		{
			pHelp->byConditionCheck = READ_BYTE(bstrData, pstrDataName->c_str());
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


sTBLDAT* HelpTable::FindData(TBLIDX tblidx)
{
	if (0 == tblidx)
		return NULL;

	TABLEIT iter;
	iter = m_mapTableList.find(tblidx);
	if (End() == iter)
		return NULL;

	return (sTBLDAT*)(iter->second);
}

bool HelpTable::LoadFromBinary(Serializer& serializer, bool bReload)
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
		sHELP_TBLDAT* pTableData = new sHELP_TBLDAT;
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

bool HelpTable::SaveToBinary(Serializer& serializer)
{
	serializer.Refresh();

	BYTE byMargin = 1;
	serializer << byMargin;

	TABLE::iterator iter;
	for (iter = Begin(); End() != iter; iter++)
	{
		sHELP_TBLDAT* pTableData = (sHELP_TBLDAT*)(iter->second);

		pTableData->SaveToBinary(serializer);
	}

	return true;
}