#include "StdAfx.h"
#include "HLSMerchantTable.h"
#include "Serializer.h"

WCHAR* HLSMerchantTable::m_pwszSheetList[] =
{
	L"Table_Data_KOR",
	NULL
};

HLSMerchantTable::HLSMerchantTable(void)
{
	Init();
}

HLSMerchantTable::~HLSMerchantTable(void)
{
	Destroy();
}

bool HLSMerchantTable::Create(DWORD dwCodePage)
{
	return Table::Create(dwCodePage);
}

void HLSMerchantTable::Destroy()
{
	Table::Destroy();
}

void HLSMerchantTable::Init()
{
}

void* HLSMerchantTable::AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sHLS_MERCHANT_TBLDAT* pMerchant = new sHLS_MERCHANT_TBLDAT;
		if (NULL == pMerchant)
			return NULL;

		CPINFO cpInfo;
		if (false == GetCPInfo(dwCodePage, &cpInfo))
		{
			delete pMerchant;
			return NULL;
		}

		m_dwCodePage = dwCodePage;
		return pMerchant;
	}

	return NULL;
}

bool HLSMerchantTable::DeallocNewTable(void* pvTable, WCHAR* pwszSheetName)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sHLS_MERCHANT_TBLDAT* pMerchant = (sHLS_MERCHANT_TBLDAT*)pvTable;
		if (FALSE != IsBadReadPtr(pMerchant, sizeof(*pMerchant)))
			return false;

		delete pMerchant;

		return true;
	}

	return false;
}

bool HLSMerchantTable::AddTable(void * pvTable, bool bReload)
{
	sHLS_MERCHANT_TBLDAT * pTbldat = (sHLS_MERCHANT_TBLDAT*)pvTable;
	sHLS_MERCHANT_TBLDAT * pExistTbldat = NULL;

	if (bReload)
	{
		pExistTbldat = (sHLS_MERCHANT_TBLDAT*)FindData(pTbldat->tblidx);
		if (pExistTbldat)
		{
			CopyMemory(pTbldat, pExistTbldat, pTbldat->GetDataSize());
			// ����Ÿ�� ������ ���� false ��ȯ
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

bool HLSMerchantTable::SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sHLS_MERCHANT_TBLDAT* pMerchant = (sHLS_MERCHANT_TBLDAT*)pvTable;

		if (0 == wcscmp(pstrDataName->c_str(), L"Tblidx"))
		{
			pMerchant->tblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Tab_Name"))
		{
			pMerchant->Tab_Name = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Name_Text"))
		{
			READ_STRINGW(bstrData, pMerchant->wszNameText, _countof(pMerchant->wszNameText));
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Tab_Type"))
		{
			pMerchant->byTabType = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Start_Item_Tblidx"))
		{
			pMerchant->startItemTblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"End_Item_Tblidx"))
		{
			pMerchant->endItemTblidx = READ_DWORD(bstrData);
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


sTBLDAT* HLSMerchantTable::FindData(TBLIDX tblidx)
{
	if (0 == tblidx)
		return NULL;

	TABLEIT iter;
	iter = m_mapTableList.find(tblidx);
	if (End() == iter)
		return NULL;

	return (sTBLDAT*)(iter->second);
}

bool HLSMerchantTable::LoadFromBinary(Serializer& serializer, bool bReload)
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
		sHLS_MERCHANT_TBLDAT* pTableData = new sHLS_MERCHANT_TBLDAT;
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

bool HLSMerchantTable::SaveToBinary(Serializer& serializer)
{
	serializer.Refresh();

	BYTE byMargin = 1;
	serializer << byMargin;

	TABLE::iterator iter;
	for (iter = Begin(); End() != iter; iter++)
	{
		sHLS_MERCHANT_TBLDAT* pTableData = (sHLS_MERCHANT_TBLDAT*)(iter->second);

		pTableData->SaveToBinary(serializer);
	}

	return true;
}