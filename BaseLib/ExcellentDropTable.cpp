#include "StdAfx.h"
#include "ExcellentDropTable.h"
#include "Serializer.h"

WCHAR* ExcellentDropTable::m_pwszSheetList[] =
{
	L"Table_Data_KOR",
	NULL
};

ExcellentDropTable::ExcellentDropTable(void)
{
	Init();
}

ExcellentDropTable::~ExcellentDropTable(void)
{
	Destroy();
}

bool ExcellentDropTable::Create(DWORD dwCodePage)
{
	return Table::Create(dwCodePage);
}

void ExcellentDropTable::Destroy()
{
	Table::Destroy();
}

void ExcellentDropTable::Init()
{
}

void* ExcellentDropTable::AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sEXCELLENT_DROP_TBLDAT* pDrop = new sEXCELLENT_DROP_TBLDAT;
		if (NULL == pDrop)
			return NULL;

		CPINFO cpInfo;
		if (false == GetCPInfo(dwCodePage, &cpInfo))
		{
			delete pDrop;
			return NULL;
		}

		m_dwCodePage = dwCodePage;
		return pDrop;
	}

	return NULL;
}

bool ExcellentDropTable::DeallocNewTable(void* pvTable, WCHAR* pwszSheetName)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sEXCELLENT_DROP_TBLDAT* pDrop = (sEXCELLENT_DROP_TBLDAT*)pvTable;
		if (FALSE != IsBadReadPtr(pDrop, sizeof(*pDrop)))
			return false;

		delete pDrop;

		return true;
	}

	return false;
}

bool ExcellentDropTable::AddTable(void * pvTable, bool bReload)
{
	sEXCELLENT_DROP_TBLDAT * pTbldat = (sEXCELLENT_DROP_TBLDAT*)pvTable;
	sEXCELLENT_DROP_TBLDAT * pExistTbldat = NULL;

	if (bReload)
	{
		pExistTbldat = (sEXCELLENT_DROP_TBLDAT*)FindData(pTbldat->tblidx);
		if (pExistTbldat)
		{
			CopyMemory(pTbldat, pExistTbldat, pTbldat->GetDataSize());
			// return false for release reloaded table data
			return true;
		}
	}

	if (false == m_mapTableList.insert(std::map<TBLIDX, sTBLDAT*>::value_type(pTbldat->tblidx, pTbldat)).second)
	{
		Table::CallErrorCallbackFunction(L"[File] : %s\r\n Table Tblidx[%u] is Duplicated ", m_wszXmlFileName, pTbldat->tblidx);
		_ASSERTE(0);
		return false;
	}

	return true;
}

bool ExcellentDropTable::SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sEXCELLENT_DROP_TBLDAT* pDrop = (sEXCELLENT_DROP_TBLDAT*)pvTable;

		if (0 == wcscmp(pstrDataName->c_str(), L"Tblidx"))
		{
			pDrop->tblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcsncmp(pstrDataName->c_str(), L"Item_Tblidx_", wcslen(L"Item_Tblidx_")))
		{
			bool bFound = false;

			WCHAR szBuffer[1024] = { 0x00, };
			for (int i = 0; i < NTL_MAX_EXCELLENT_DROP; i++)
			{
				swprintf(szBuffer, 1024, L"Item_Tblidx_%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					pDrop->aItem_Tblidx[i] = READ_DWORD(bstrData);

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
		else if (0 == wcsncmp(pstrDataName->c_str(), L"Drop_Rate_", wcslen(L"Drop_Rate_")))
		{
			bool bFound = false;

			WCHAR szBuffer[1024] = { 0x00, };
			for (int i = 0; i < NTL_MAX_EXCELLENT_DROP; i++)
			{
				swprintf(szBuffer, 1024, L"Drop_Rate_%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					pDrop->afDrop_Rate[i] = READ_FLOAT(bstrData, pstrDataName->c_str(), 0.0f);

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


sTBLDAT* ExcellentDropTable::FindData(TBLIDX tblidx)
{
	if (0 == tblidx)
		return NULL;

	TABLEIT iter;
	iter = m_mapTableList.find(tblidx);
	if (End() == iter)
		return NULL;

	return (sTBLDAT*)(iter->second);
}

TBLIDX ExcellentDropTable::FindDropIndex(sEXCELLENT_DROP_TBLDAT* psTblData, BYTE byIndex)
{
	if (NTL_MAX_EXCELLENT_DROP <= byIndex || 0 > byIndex)
		return INVALID_TBLIDX;

	return psTblData->aItem_Tblidx[byIndex];
}

float ExcellentDropTable::FindDropRate(sEXCELLENT_DROP_TBLDAT* psTblData, BYTE byIndex)
{
	if (NTL_MAX_EXCELLENT_DROP <= byIndex || 0 > byIndex)
		return 0.0f;

	return psTblData->afDrop_Rate[byIndex];
}

bool ExcellentDropTable::LoadFromBinary(Serializer& serializer, bool bReload)
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
		sEXCELLENT_DROP_TBLDAT* pTableData = new sEXCELLENT_DROP_TBLDAT;
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

bool ExcellentDropTable::SaveToBinary(Serializer& serializer)
{
	serializer.Refresh();

	BYTE byMargin = 1;
	serializer << byMargin;

	TABLE::iterator iter;
	for (iter = Begin(); End() != iter; iter++)
	{
		sEXCELLENT_DROP_TBLDAT* pTableData = (sEXCELLENT_DROP_TBLDAT*)(iter->second);

		pTableData->SaveToBinary(serializer);
	}

	return true;
}