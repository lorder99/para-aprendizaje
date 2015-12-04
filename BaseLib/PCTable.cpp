#include "stdafx.h"
#include "PCTable.h"
#include "Serializer.h"

WCHAR* PCTable::m_pwszSheetList[] =
{
	L"Table_Data_KOR",
	NULL
};

PCTable::PCTable(void)
{
	Init();
}

PCTable::~PCTable(void)
{
	Destroy();
}

bool PCTable::Create(DWORD dwCodePage)
{
	return Table::Create(dwCodePage);
}

void PCTable::Destroy()
{
	::ZeroMemory(m_aPcTbldat, sizeof(m_aPcTbldat));

	Table::Destroy();
}

void PCTable::Init()
{
	ZeroMemory(m_aPcTbldat, sizeof(m_aPcTbldat));
}

void* PCTable::AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sPC_TBLDAT* pNewPC = new sPC_TBLDAT;
		if (NULL == pNewPC)
			return NULL;

		CPINFO cpInfo;
		if (false == GetCPInfo(dwCodePage, &cpInfo))
		{
			delete pNewPC;
			return NULL;
		}

		m_dwCodePage = dwCodePage;
		return pNewPC;
	}

	return NULL;
}

bool PCTable::DeallocNewTable(void* pvTable, WCHAR* pwszSheetName)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sPC_TBLDAT* pPC = (sPC_TBLDAT*)pvTable;
		if (FALSE != IsBadReadPtr(pPC, sizeof(*pPC)))
			return false;

		delete pPC;

		return true;
	}

	return false;
}

bool PCTable::AddTable(void * pvTable, bool bReload)
{
	UNREFERENCED_PARAMETER(bReload);

	sPC_TBLDAT * pTbldat = (sPC_TBLDAT*)pvTable;

	if (false == m_mapTableList.insert(std::pair<TBLIDX, sTBLDAT*>(pTbldat->tblidx, pTbldat)).second)
	{
		Table::CallErrorCallbackFunction(L"[File] : %s\r\n Table Tblidx[%u] is Duplicated ", m_wszXmlFileName, pTbldat->tblidx);
		_ASSERTE(0);
		return false;
	}

	//  [4/17/2006 zeroera] : �߰� : race, class, gender�� ���� tbldat ����
	if (false == SetPcTbldat(pTbldat->byRace, pTbldat->byClass, pTbldat->byGender, pTbldat))
	{
		_ASSERTE(0);
		m_mapTableList.erase(pTbldat->tblidx);
		return false;
	}


	return true;
}

bool PCTable::SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData)
{
	sPC_TBLDAT* pPC = (sPC_TBLDAT*)pvTable;

	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		if (0 == wcscmp(pstrDataName->c_str(), L"Tblidx"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->tblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Race"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->byRace = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Gender"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->byGender = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Class"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->byClass = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Prior_Class_Tblidx"))
		{
			pPC->prior_Class_Tblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Class_Bit_Flag"))
		{
			pPC->dwClass_Bit_Flag = READ_BITFLAG(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Model_Child"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);

			READ_STRING(bstrData, pPC->szModel_Child, _countof(pPC->szModel_Child));
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Model_Adult"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);

			READ_STRING(bstrData, pPC->szModel_Adult, _countof(pPC->szModel_Adult));
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Basic_LP"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->wBasic_LP = READ_WORD(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Basic_EP"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->wBasic_EP = READ_WORD(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Basic_RP"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->wBasic_RP = READ_WORD(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Basic_Physical_Offence"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->wBasic_Physical_Offence = READ_WORD(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Basic_Physical_Defence"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->wBasic_Physical_Defence = READ_WORD(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Basic_Energy_Offence"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->wBasic_Energy_Offence = READ_WORD(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Basic_Energy_Defence"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->wBasic_Energy_Defence = READ_WORD(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Basic_Energy_Defence"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->wBasic_Energy_Defence = READ_WORD(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Basic_Str"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->byStr = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Basic_Con"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->byCon = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Basic_Foc"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->byFoc = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Basic_Dex"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->byDex = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Basic_Sol"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->bySol = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Basic_Eng"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->byEng = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Level_Up_LP"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->byLevel_Up_LP = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Level_Up_EP"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->byLevel_Up_EP = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Level_Up_RP"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->byLevel_Up_RP = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Level_Up_Physical_Offence"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->byLevel_Up_Physical_Offence = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Level_Up_Physical_Defence"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->byLevel_Up_Physical_Defence = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Level_Up_Energy_Offence"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->byLevel_Up_Energy_Offence = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Level_Up_Energy_Defence"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->byLevel_Up_Energy_Defence = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Level_Up_Str"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->fLevel_Up_Str = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Level_Up_Con"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->fLevel_Up_Con = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Level_Up_Foc"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->fLevel_Up_Foc = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Level_Up_Dex"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->fLevel_Up_Dex = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Level_Up_Sol"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->fLevel_Up_Sol = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Level_Up_Eng"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->fLevel_Up_Eng = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Scale"))
		{
			pPC->fScale = READ_FLOAT(bstrData, pstrDataName->c_str(), 1.0f);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Child_Run_Speed_Origin"))
		{
			pPC->fChild_Run_Speed_Origin = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Child_Run_Speed"))
		{
			pPC->fChild_Run_Speed = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Adult_Run_Speed_Origin"))
		{
			pPC->fAdult_Run_Speed_Origin = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Adult_Run_Speed"))
		{
			pPC->fAdult_Run_Speed = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Radius"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->fRadius = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Attack_Speed_Rate"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->wAttack_Speed_Rate = READ_WORD(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Attack_Type"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->byAttack_Type = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Attack_Range"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->fAttack_Range = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Basic_Attack_Rate"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->wAttack_Rate = READ_WORD(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Basic_Dodge_Rate"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->wDodge_Rate = READ_WORD(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Basic_Block_Rate"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->wBlock_Rate = READ_WORD(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Basic_Curse_Success_Rate"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->wCurse_Success_Rate = READ_WORD(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Basic_Curse_Tolerance_Rate"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);
			pPC->wCurse_Tolerance_Rate = READ_WORD(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Basic_Aggro_Point"))
		{
			pPC->wBasic_Aggro_Point = READ_WORD(bstrData, pstrDataName->c_str(), 0);
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


sTBLDAT* PCTable::FindData(TBLIDX tblidx)
{
	if (0 == tblidx)
		return NULL;

	TABLEIT iter;
	iter = m_mapTableList.find(tblidx);
	if (End() == iter)
		return NULL;

	return (sTBLDAT*)(iter->second);
}


//-----------------------------------------------------------------------------------
//		Purpose	:	
//		Return	:	race, class, gender�� ���� table�� ��ȯ
//-----------------------------------------------------------------------------------
sTBLDAT * PCTable::GetPcTbldat(BYTE byRace, BYTE byClass, BYTE byGender)
{
	if (byRace > RACE_LAST)
	{
		return NULL;
	}

	if (byClass > PC_CLASS_LAST)
	{
		return NULL;
	}

	if (byGender > GENDER_LAST)
	{
		return NULL;
	}


	return m_aPcTbldat[byRace][byClass][byGender];
}


//-----------------------------------------------------------------------------------
//		Purpose	:
//		Return	:
//-----------------------------------------------------------------------------------
bool PCTable::SetPcTbldat(BYTE byRace, BYTE byClass, BYTE byGender, sTBLDAT * pTbldat)
{
	if (byRace > RACE_LAST)
	{
		return false;
	}

	if (byClass > PC_CLASS_LAST)
	{
		return false;
	}

	if (byGender > GENDER_LAST)
	{
		return false;
	}

	// already registered?
	if (NULL != m_aPcTbldat[byRace][byClass][byGender])
	{
		return false;
	}

	m_aPcTbldat[byRace][byClass][byGender] = pTbldat;

	return true;
}

bool PCTable::LoadFromBinary(Serializer& serializer, bool bReload)
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
		sPC_TBLDAT* pTableData = new sPC_TBLDAT;
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

bool PCTable::SaveToBinary(Serializer& serializer)
{
	serializer.Refresh();

	BYTE byMargin = 1;
	serializer << byMargin;

	TABLE::iterator iter;
	for (iter = Begin(); End() != iter; iter++)
	{
		sPC_TBLDAT* pTableData = (sPC_TBLDAT*)(iter->second);

		pTableData->SaveToBinary(serializer);
	}

	return true;
}
