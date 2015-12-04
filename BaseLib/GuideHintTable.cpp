#include "StdAfx.h"
#include "GuideHintTable.h"
#include "Serializer.h"

WCHAR* GuideHintTable::m_pwszSheetList[] =
{
	L"Table_Data_KOR",
	NULL
};

GuideHintTable::GuideHintTable(void)
{
	Init();
}

GuideHintTable::~GuideHintTable(void)
{
	Destroy();
}

bool GuideHintTable::Create(DWORD dwCodePage)
{
	return Table::Create(dwCodePage);
}

void GuideHintTable::Destroy()
{
	Table::Destroy();
}

void GuideHintTable::Init()
{
}

void* GuideHintTable::AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sGUIDE_HINT_TBLDAT* pGuide = new sGUIDE_HINT_TBLDAT;
		if (NULL == pGuide)
			return NULL;

		CPINFO cpInfo;
		if (false == GetCPInfo(dwCodePage, &cpInfo))
		{
			delete pGuide;
			return NULL;
		}

		m_dwCodePage = dwCodePage;
		return pGuide;
	}

	return NULL;
}

bool GuideHintTable::DeallocNewTable(void* pvTable, WCHAR* pwszSheetName)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sGUIDE_HINT_TBLDAT* pGuide = (sGUIDE_HINT_TBLDAT*)pvTable;
		if (FALSE != IsBadReadPtr(pGuide, sizeof(*pGuide)))
			return false;

		delete pGuide;

		return true;
	}

	return false;
}

bool GuideHintTable::AddTable(void * pvTable, bool bReload)
{
	UNREFERENCED_PARAMETER(bReload);

	sGUIDE_HINT_TBLDAT* pTbldat = (sGUIDE_HINT_TBLDAT*)pvTable;

	if (false == m_mapTableList.insert(std::pair<TBLIDX, sTBLDAT*>(pTbldat->tblidx, pTbldat)).second)
	{
		Table::CallErrorCallbackFunction(L"[File] : %s\r\n Table Tblidx[%u] is Duplicated ", m_wszXmlFileName, pTbldat->tblidx);
		_ASSERTE(0);
		return false;
	}


	return true;
}

bool GuideHintTable::SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sGUIDE_HINT_TBLDAT* pGuide = (sGUIDE_HINT_TBLDAT*)pvTable;

		if (0 == wcscmp(pstrDataName->c_str(), L"Tblidx"))
		{
			pGuide->tblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Type"))
		{
			pGuide->byType = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"WidthPosition"))
		{
			pGuide->byWidthPosition = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"HeightPosition"))
		{
			pGuide->byHeightPosition = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"WidthSize"))
		{
			pGuide->wWidthSize = READ_WORD(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"HeightSize"))
		{
			pGuide->wHeightSize = READ_WORD(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Resource"))
		{
			READ_STRING(bstrData, pGuide->szResource, _countof(pGuide->szResource));
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Note"))
		{
			READ_STRING(bstrData, pGuide->szNote, _countof(pGuide->szNote));
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Auto_Show"))
		{
			pGuide->bAutoShow = READ_BOOL(bstrData, pstrDataName->c_str());
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


sTBLDAT* GuideHintTable::FindData(TBLIDX tblidx)
{
	if (0 == tblidx)
		return NULL;

	TABLEIT iter;
	iter = m_mapTableList.find(tblidx);
	if (End() == iter)
		return NULL;

	return (sTBLDAT*)(iter->second);
}

bool GuideHintTable::LoadFromBinary(Serializer& serializer, bool bReload)
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
		sGUIDE_HINT_TBLDAT* pTableData = new sGUIDE_HINT_TBLDAT;
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

bool GuideHintTable::SaveToBinary(Serializer& serializer)
{
	serializer.Refresh();

	BYTE byMargin = 1;
	serializer << byMargin;

	TABLE::iterator iter;
	for (iter = Begin(); End() != iter; iter++)
	{
		sGUIDE_HINT_TBLDAT* pTableData = (sGUIDE_HINT_TBLDAT*)(iter->second);

		pTableData->SaveToBinary(serializer);
	}

	return true;
}