#include "StdAfx.h"
#include "StatusTransformTable.h"
#include "Serializer.h"


WCHAR* StatusTransformTable::m_pwszSheetList[] =
{
	L"Table_Data_KOR",
	NULL
};

StatusTransformTable::StatusTransformTable(void)
{
	Init();
}

StatusTransformTable::~StatusTransformTable(void)
{
	Destroy();
}

bool StatusTransformTable::Create(DWORD dwCodePage)
{
	return Table::Create(dwCodePage);
}

void StatusTransformTable::Destroy()
{
	Table::Destroy();
}

void StatusTransformTable::Init()
{
}

void* StatusTransformTable::AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sSTATUS_TRANSFORM_TBLDAT* pNewStatusTransform = new sSTATUS_TRANSFORM_TBLDAT;
		if (NULL == pNewStatusTransform)
		{
			return NULL;
		}

		CPINFO cpInfo;
		if (false == GetCPInfo(dwCodePage, &cpInfo))
		{
			delete pNewStatusTransform;
			return NULL;
		}

		m_dwCodePage = dwCodePage;
		return pNewStatusTransform;
	}

	return NULL;
}

bool StatusTransformTable::DeallocNewTable(void* pvTable, WCHAR* pwszSheetName)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sSTATUS_TRANSFORM_TBLDAT* pStatusTransform = (sSTATUS_TRANSFORM_TBLDAT*)pvTable;
		if (FALSE != IsBadReadPtr(pStatusTransform, sizeof(*pStatusTransform)))
		{
			return false;
		}

		delete pStatusTransform;

		return true;
	}

	return false;
}

bool StatusTransformTable::AddTable(void * pvTable, bool bReload)
{
	UNREFERENCED_PARAMETER(bReload);

	sSTATUS_TRANSFORM_TBLDAT* pTbldat = (sSTATUS_TRANSFORM_TBLDAT*)pvTable;

	if (false == m_mapTableList.insert(std::pair<TBLIDX, sTBLDAT*>(pTbldat->tblidx, pTbldat)).second)
	{
		Table::CallErrorCallbackFunction(L"[File] : %s\r\n Table Tblidx[%u] is Duplicated ", m_wszXmlFileName, pTbldat->tblidx);
		_ASSERTE(0);
		return false;
	}

	return true;
}

bool StatusTransformTable::SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData)
{
	sSTATUS_TRANSFORM_TBLDAT* pStatusTransform = (sSTATUS_TRANSFORM_TBLDAT*)pvTable;

	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		if (0 == wcscmp(pstrDataName->c_str(), L"Tblidx"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pStatusTransform->tblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"LP_Transform"))
		{
			pStatusTransform->fLP_Transform = READ_FLOAT(bstrData, pstrDataName->c_str(), 1.0f);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"EP_Transform"))
		{
			pStatusTransform->fEP_Transform = READ_FLOAT(bstrData, pstrDataName->c_str(), 1.0f);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Physical_Offence_Transform"))
		{
			pStatusTransform->fPhysical_Offence_Transform = READ_FLOAT(bstrData, pstrDataName->c_str(), 1.0f);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Energy_Offence_Transform"))
		{
			pStatusTransform->fEnergy_Offence_Transform = READ_FLOAT(bstrData, pstrDataName->c_str(), 1.0f);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Physical_Defence_Transform"))
		{
			pStatusTransform->fPhysical_Defence_Transform = READ_FLOAT(bstrData, pstrDataName->c_str(), 1.0f);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Energy_Defence_Transform"))
		{
			pStatusTransform->fEnergy_Defence_Transform = READ_FLOAT(bstrData, pstrDataName->c_str(), 1.0f);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Run_Speed_Transform"))
		{
			pStatusTransform->fRun_Speed_Transform = READ_FLOAT(bstrData, pstrDataName->c_str(), 1.0f);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Attack_Speed_Transform"))
		{
			pStatusTransform->fAttack_Speed_Transform = READ_FLOAT(bstrData, pstrDataName->c_str(), 1.0f);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Attack_Rate_Transform"))
		{
			pStatusTransform->fAttack_Rate_Transform = READ_FLOAT(bstrData, pstrDataName->c_str(), 1.0f);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Dodge_Rate_Transform"))
		{
			pStatusTransform->fDodge_Rate_Transform = READ_FLOAT(bstrData, pstrDataName->c_str(), 1.0f);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Block_Rate_Transform"))
		{
			pStatusTransform->fBlock_Rate_Transform = READ_FLOAT(bstrData, pstrDataName->c_str(), 1.0f);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Curse_Success_Transform"))
		{
			pStatusTransform->fCurse_Success_Transform = READ_FLOAT(bstrData, pstrDataName->c_str(), 1.0f);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Curse_Tolerance_Transform"))
		{
			pStatusTransform->fCurse_Tolerance_Transform = READ_FLOAT(bstrData, pstrDataName->c_str(), 1.0f);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Attack_Range_Change"))
		{
			pStatusTransform->fAttack_Range_Change = READ_FLOAT(bstrData, pstrDataName->c_str(), 0.0f);
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

sTBLDAT* StatusTransformTable::FindData(TBLIDX tblidx)
{
	if (0 == tblidx)
		return NULL;

	TABLEIT iter;
	iter = m_mapTableList.find(tblidx);
	if (End() == iter)
		return NULL;

	return (sTBLDAT*)(iter->second);
}

bool StatusTransformTable::LoadFromBinary(Serializer& serializer, bool bReload)
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
		sSTATUS_TRANSFORM_TBLDAT* pTableData = new sSTATUS_TRANSFORM_TBLDAT;
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

bool StatusTransformTable::SaveToBinary(Serializer& serializer)
{
	serializer.Refresh();

	BYTE byMargin = 1;
	serializer << byMargin;

	TABLE::iterator iter;
	for (iter = Begin(); End() != iter; iter++)
	{
		sSTATUS_TRANSFORM_TBLDAT* pTableData = (sSTATUS_TRANSFORM_TBLDAT*)(iter->second);

		pTableData->SaveToBinary(serializer);
	}

	return true;
}