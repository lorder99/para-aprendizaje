#include "stdafx.h"
#include "ItemRecipeTable.h"
#include "NtlBattle.h"
#include "Serializer.h"

WCHAR* ItemRecipeTable::m_pwszSheetList[] =
{
	L"Table_Data_KOR",
	NULL
};

ItemRecipeTable::ItemRecipeTable(void)
{
	Init();
}

ItemRecipeTable::~ItemRecipeTable(void)
{
	Destroy();
}

bool ItemRecipeTable::Create(DWORD dwCodePage)
{
	return Table::Create(dwCodePage);
}

void ItemRecipeTable::Destroy()
{
	Table::Destroy();
}

void ItemRecipeTable::Init()
{
}

void* ItemRecipeTable::AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sITEM_RECIPE_TBLDAT* pNewItem = new sITEM_RECIPE_TBLDAT;
		if (NULL == pNewItem)
			return NULL;

		CPINFO cpInfo;
		if (false == GetCPInfo(dwCodePage, &cpInfo))
		{
			delete pNewItem;
			return NULL;
		}

		m_dwCodePage = dwCodePage;
		return pNewItem;
	}

	return NULL;
}

bool ItemRecipeTable::DeallocNewTable(void* pvTable, WCHAR* pwszSheetName)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sITEM_RECIPE_TBLDAT* pItem = (sITEM_RECIPE_TBLDAT*)pvTable;
		if (FALSE != IsBadReadPtr(pItem, sizeof(*pItem)))
			return false;

		delete pItem;

		return true;
	}

	return false;
}

bool ItemRecipeTable::AddTable(void * pvTable, bool bReload)
{
	sITEM_RECIPE_TBLDAT * pTbldat = (sITEM_RECIPE_TBLDAT*)pvTable;
	sITEM_RECIPE_TBLDAT * pExistTbldat = NULL;

	if (bReload)
	{
		pExistTbldat = (sITEM_RECIPE_TBLDAT*)FindData(pTbldat->tblidx);
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

bool ItemRecipeTable::SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData)
{

	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sITEM_RECIPE_TBLDAT* pItem = (sITEM_RECIPE_TBLDAT*)pvTable;

		if (0 == wcscmp(pstrDataName->c_str(), L"Tblidx"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pItem->tblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Validity_Able"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pItem->bValidityAble = READ_BOOL(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Name"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pItem->dwName = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Recipe_Type"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pItem->byRecipeType = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Need_Mix_Level"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pItem->byNeedMixLevel = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Need_Mix_Zenny"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pItem->dwNeedMixZenny = READ_DWORD(bstrData);
		}
		else if (0 == wcsncmp(pstrDataName->c_str(), L"Create_Item_Tblidx_", wcslen(L"Create_Item_Tblidx_")))
		{
			bool bFound = false;

			WCHAR szBuffer[1024] = { 0x00, };
			for (int i = 0; i < DBO_MAX_COUNT_RECIPE_CREATE_ITEM; i++)
			{
				swprintf(szBuffer, 1024, L"Create_Item_Tblidx_%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					if (0 == i)
					{
						CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
					}
					pItem->asCreateItemTblidx[i].itemTblidx = READ_DWORD(bstrData);

					bFound = true;
					break;
				}
			}

			for (int i = 0; i < DBO_MAX_COUNT_RECIPE_CREATE_ITEM; i++)
			{
				swprintf(szBuffer, 1024, L"Create_Item_Tblidx_Greate_%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					if (0 == i)
					{
						CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
					}
					pItem->asCreateItemTblidx[i].itemTblidxGreate = READ_DWORD(bstrData);

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
		else if (0 == wcsncmp(pstrDataName->c_str(), L"Create_Item_Rate_", wcslen(L"Create_Item_Rate_")))
		{
			bool bFound = false;

			WCHAR szBuffer[1024] = { 0x00, };
			for (int i = 0; i < DBO_MAX_COUNT_RECIPE_CREATE_ITEM; i++)
			{
				swprintf(szBuffer, 1024, L"Create_Item_Rate_%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					if (0 == i)
					{
						CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
					}
					pItem->asCreateItemTblidx[i].itemRate = READ_BYTE(bstrData, pstrDataName->c_str());

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
		else if (0 == wcscmp(pstrDataName->c_str(), L"Create_Junk_Tblidx"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pItem->createJunkTblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcsncmp(pstrDataName->c_str(), L"Material_Tblidx_", wcslen(L"Material_Tblidx_")))
		{
			bool bFound = false;

			WCHAR szBuffer[1024] = { 0x00, };
			for (int i = 0; i < DBO_MAX_COUNT_RECIPE_MATERIAL_ITEM; i++)
			{
				swprintf(szBuffer, 1024, L"Material_Tblidx_%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					if (i == 0)
					{
						CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
					}
					pItem->asMaterial[i].materialTblidx = READ_DWORD(bstrData);

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
		else if (0 == wcsncmp(pstrDataName->c_str(), L"Material_Quantity_", wcslen(L"Material_Quantity_")))
		{
			bool bFound = false;

			WCHAR szBuffer[1024] = { 0x00, };
			for (int i = 0; i < DBO_MAX_COUNT_RECIPE_MATERIAL_ITEM; i++)
			{
				swprintf(szBuffer, 1024, L"Material_Quantity_%d", i + 1);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					if (i == 0)
					{
						CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
					}
					pItem->asMaterial[i].byMaterialCount = READ_BYTE(bstrData, pstrDataName->c_str());

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


sTBLDAT* ItemRecipeTable::FindData(TBLIDX tblidx)
{
	if (0 == tblidx)
		return NULL;

	TABLEIT iter;
	iter = m_mapTableList.find(tblidx);
	if (End() == iter)
		return NULL;

	return (sTBLDAT*)(iter->second);
}

bool ItemRecipeTable::LoadFromBinary(Serializer& serializer, bool bReload)
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
		sITEM_RECIPE_TBLDAT* pTableData = new sITEM_RECIPE_TBLDAT;
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

bool ItemRecipeTable::SaveToBinary(Serializer& serializer)
{
	serializer.Refresh();

	BYTE byMargin = 1;
	serializer << byMargin;

	TABLE::iterator iter;
	for (iter = Begin(); End() != iter; iter++)
	{
		sITEM_RECIPE_TBLDAT* pTableData = (sITEM_RECIPE_TBLDAT*)(iter->second);

		pTableData->SaveToBinary(serializer);
	}

	return true;
}