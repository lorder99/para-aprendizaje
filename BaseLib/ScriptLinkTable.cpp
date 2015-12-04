#include "StdAfx.h"
#include "ScriptLinkTable.h"
#include "Serializer.h"

WCHAR* ScriptLinkTable::m_pwszSheetList[] =
{
	L"Table_Data_KOR",
	NULL
};

ScriptLinkTable::ScriptLinkTable(void)
{
	Init();
}

ScriptLinkTable::~ScriptLinkTable(void)
{
	Destroy();
}

bool ScriptLinkTable::Create(DWORD dwCodePage)
{
	return Table::Create(dwCodePage);
}

void ScriptLinkTable::Destroy()
{
	Table::Destroy();
}

void ScriptLinkTable::Init()
{
}

void* ScriptLinkTable::AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sSCRIPT_LINK_TBLDAT* pLink = new sSCRIPT_LINK_TBLDAT;
		if (NULL == pLink)
			return NULL;

		CPINFO cpInfo;
		if (false == GetCPInfo(dwCodePage, &cpInfo))
		{
			delete pLink;
			return NULL;
		}

		m_dwCodePage = dwCodePage;
		return pLink;
	}

	return NULL;
}

bool ScriptLinkTable::DeallocNewTable(void* pvTable, WCHAR* pwszSheetName)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sSCRIPT_LINK_TBLDAT* pLink = (sSCRIPT_LINK_TBLDAT*)pvTable;
		if (FALSE != IsBadReadPtr(pLink, sizeof(*pLink)))
			return false;

		delete pLink;

		return true;
	}

	return false;
}

bool ScriptLinkTable::AddTable(void * pvTable, bool bReload)
{
	UNREFERENCED_PARAMETER(bReload);

	sSCRIPT_LINK_TBLDAT* pTbldat = (sSCRIPT_LINK_TBLDAT*)pvTable;

	if (false == m_mapTableList.insert(std::pair<TBLIDX, sTBLDAT*>(pTbldat->tblidx, pTbldat)).second)
	{
		Table::CallErrorCallbackFunction(L"[File] : %s\r\n Table Tblidx[%u] is Duplicated ", m_wszXmlFileName, pTbldat->tblidx);
		_ASSERTE(0);
		return false;
	}


	return true;
}

bool ScriptLinkTable::SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sSCRIPT_LINK_TBLDAT* pLink = (sSCRIPT_LINK_TBLDAT*)pvTable;

		if (0 == wcscmp(pstrDataName->c_str(), L"Tblidx"))
		{
			pLink->tblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Value"))
		{
			READ_STRINGW(bstrData, pLink->wszValue, _countof(pLink->wszValue));
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Type"))
		{
			pLink->byType = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Action"))
		{
			pLink->byAction = READ_BYTE(bstrData, pstrDataName->c_str());
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


sTBLDAT* ScriptLinkTable::FindData(TBLIDX tblidx)
{
	if (0 == tblidx)
		return NULL;

	TABLEIT iter;
	iter = m_mapTableList.find(tblidx);
	if (End() == iter)
		return NULL;

	return (sTBLDAT*)(iter->second);
}

bool ScriptLinkTable::LoadFromBinary(Serializer& serializer, bool bReload)
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
		sSCRIPT_LINK_TBLDAT* pTableData = new sSCRIPT_LINK_TBLDAT;
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

bool ScriptLinkTable::SaveToBinary(Serializer& serializer)
{
	serializer.Refresh();

	BYTE byMargin = 1;
	serializer << byMargin;

	TABLE::iterator iter;
	for (iter = Begin(); End() != iter; iter++)
	{
		sSCRIPT_LINK_TBLDAT* pTableData = (sSCRIPT_LINK_TBLDAT*)(iter->second);

		pTableData->SaveToBinary(serializer);
	}

	return true;
}