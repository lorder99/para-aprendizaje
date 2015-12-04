#include "StdAfx.h"
#include "LandMarkTable.h"
#include "Serializer.h"


WCHAR* LandMarkTable::m_pwszSheetList[] =
{
	L"Table_Data_KOR",
	NULL
};

LandMarkTable::LandMarkTable(void)
{
	Init();
}

LandMarkTable::~LandMarkTable(void)
{
	Destroy();
}

bool LandMarkTable::Create(DWORD dwCodePage)
{
	return Table::Create(dwCodePage);
}

void LandMarkTable::Destroy()
{
	Table::Destroy();
}

void LandMarkTable::Init()
{
}

void* LandMarkTable::AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sLAND_MARK_TBLDAT* pLand = new sLAND_MARK_TBLDAT;
		if (NULL == pLand)
			return NULL;

		CPINFO cpInfo;
		if (false == GetCPInfo(dwCodePage, &cpInfo))
		{
			delete pLand;
			return NULL;
		}

		m_dwCodePage = dwCodePage;
		return pLand;
	}

	return NULL;
}

bool LandMarkTable::DeallocNewTable(void* pvTable, WCHAR* pwszSheetName)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sLAND_MARK_TBLDAT* pLand = (sLAND_MARK_TBLDAT*)pvTable;
		if (FALSE != IsBadReadPtr(pLand, sizeof(*pLand)))
			return false;

		delete pLand;

		return true;
	}

	return false;
}

bool LandMarkTable::AddTable(void * pvTable, bool bReload)
{
	UNREFERENCED_PARAMETER(bReload);

	sLAND_MARK_TBLDAT* pTbldat = (sLAND_MARK_TBLDAT*)pvTable;

	if (false == m_mapTableList.insert(std::pair<TBLIDX, sTBLDAT*>(pTbldat->tblidx, pTbldat)).second)
	{
		Table::CallErrorCallbackFunction(L"[File] : %s\r\n Table Tblidx[%u] is Duplicated ", m_wszXmlFileName, pTbldat->tblidx);
		_ASSERTE(0);
		return false;
	}


	return true;
}

bool LandMarkTable::SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData)
{

	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sLAND_MARK_TBLDAT* pLand = (sLAND_MARK_TBLDAT*)pvTable;

		if (0 == wcscmp(pstrDataName->c_str(), L"Tblidx"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pLand->tblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Landmark_Name"))
		{
			pLand->LandmarkName = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Landmark_Type"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pLand->byLandmarkType = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Name_Text"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			READ_STRINGW(bstrData, pLand->wszNameText, _countof(pLand->wszNameText));
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Validity_Able"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pLand->bValidityAble = READ_BOOL(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Landmark_BitFlag"))
		{
			pLand->byLandmarkBitflag = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Landmark_Display_BitFlag"))
		{
			pLand->byLandmarkDisplayBitFlag = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Landmark_Loc_X"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pLand->LandmarkLoc.x = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Landmark_Loc_Z"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pLand->LandmarkLoc.z = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Link_Map_Idx"))
		{
			pLand->LinkMapIdx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Link_Warfog_Idx"))
		{
			pLand->wLinkWarfogIdx = READ_WORD(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Icon_Name"))
		{
			READ_STRINGW(bstrData, pLand->wszIconName, _countof(pLand->wszIconName));
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Icon_Size"))
		{
			pLand->byIconSize = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Note"))
		{
			pLand->Note = READ_DWORD(bstrData);
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


sTBLDAT* LandMarkTable::FindData(TBLIDX tblidx)
{
	if (0 == tblidx)
		return NULL;

	TABLEIT iter;
	iter = m_mapTableList.find(tblidx);
	if (End() == iter)
		return NULL;

	return (sTBLDAT*)(iter->second);
}

bool LandMarkTable::LoadFromBinary(Serializer& serializer, bool bReload)
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
		sLAND_MARK_TBLDAT* pTableData = new sLAND_MARK_TBLDAT;
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

bool LandMarkTable::SaveToBinary(Serializer& serializer)
{
	serializer.Refresh();

	BYTE byMargin = 1;
	serializer << byMargin;

	TABLE::iterator iter;
	for (iter = Begin(); End() != iter; iter++)
	{
		sLAND_MARK_TBLDAT* pTableData = (sLAND_MARK_TBLDAT*)(iter->second);

		pTableData->SaveToBinary(serializer);
	}

	return true;
}