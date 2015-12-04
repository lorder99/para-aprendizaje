#include "stdafx.h"
#include "ObjectTable.h"
#include "Serializer.h"

WCHAR* ObjectTable::m_pwszSheetList[] =
{
	L"Table_Data_KOR",
	NULL
};

ObjectTable::ObjectTable(void)
{
	Init();
}

ObjectTable::~ObjectTable(void)
{
	Destroy();
}

bool ObjectTable::Create(DWORD dwCodePage)
{
	return Table::Create(dwCodePage);
}

void ObjectTable::Destroy(void)
{
	Table::Destroy();
}

void ObjectTable::Init(void)
{
	m_dwObjectSequence = 0;
}

void* ObjectTable::AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sOBJECT_TBLDAT* pNewObj = new sOBJECT_TBLDAT;
		if (NULL == pNewObj) return NULL;

		CPINFO cpInfo;
		if (!GetCPInfo(dwCodePage, &cpInfo))
		{
			delete pNewObj;
			return NULL;
		}

		m_dwCodePage = dwCodePage;
		return pNewObj;
	}

	return NULL;
}

bool ObjectTable::DeallocNewTable(void* pvTable, WCHAR* pwszSheetName)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sOBJECT_TBLDAT* pObj = (sOBJECT_TBLDAT*)pvTable;
		if (IsBadReadPtr(pObj, sizeof(*pObj))) return false;

		delete pObj;

		return true;
	}

	return false;
}

bool ObjectTable::AddTable(void * pvTable, bool bReload)
{
	sOBJECT_TBLDAT * pTbldat = (sOBJECT_TBLDAT*)pvTable;
	sOBJECT_TBLDAT * pExistTbldat = NULL;

	// direction �缳��
	if (CNtlVector::ZERO == pTbldat->vDir)
	{
		pTbldat->vDir = CNtlVector::UNIT_X;
	}
	else
	{
		if (false == pTbldat->vDir.SafeNormalize())
		{
			_ASSERTE(0);
			return false;
		}
	}

	// radius ���
	pTbldat->fRadius = ((pTbldat->vMax - pTbldat->vMin) * 0.5).Length();


	if (bReload)
	{
		pExistTbldat = (sOBJECT_TBLDAT*)FindData(pTbldat->tblidx);
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


	// Object�� Sequence ����
	pTbldat->dwSequence = m_dwObjectSequence++;


	return true;
}

bool ObjectTable::SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData)
{
#define READ_STATE( idx )															\
	if ( READ_STRING( bstrData, szTemp, 1024 ) )									\
		{																				\
		int nCnt = 0;																\
		char *pToken, *pNextToken;													\
																					\
		pToken = strtok_s( szTemp, ";", &pNextToken );								\
																					\
																							while ( NULL != pToken )													\
																									{																			\
			DWORD dwTemp = (DWORD)_atoi64( pToken );								\
																					\
			if ( dwTemp >= INVALID_BYTE )											\
						{																		\
				_ASSERTE( !"BYTE Ÿ���� ����Ÿ ���� �ִ밪 �� �ʰ��߽��ϴ�." );		\
						}																		\
						else																	\
			{																		\
				pObj->abyState[idx][nCnt] = (BYTE)dwTemp;							\
			}																		\
																					\
			nCnt++;																	\
																					\
			pToken = strtok_s( NULL, ";", &pNextToken );							\
																									}																			\
		}

	static char szTemp[1024];

	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sOBJECT_TBLDAT* pObj = (sOBJECT_TBLDAT*)pvTable;

		if (0 == wcscmp(pstrDataName->c_str(), L"Tblidx"))
		{
			pObj->tblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Name"))
		{
			pObj->dwName = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Loc_X"))
		{
			pObj->vLoc.x = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Loc_Y"))
		{
			pObj->vLoc.y = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Loc_Z"))
		{
			pObj->vLoc.z = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Dir_X"))
		{
			pObj->vDir.x = READ_FLOAT(bstrData, pstrDataName->c_str(), 0.0f);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Dir_Y"))
		{
			pObj->vDir.y = READ_FLOAT(bstrData, pstrDataName->c_str(), 0.0f);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Dir_Z"))
		{
			pObj->vDir.z = READ_FLOAT(bstrData, pstrDataName->c_str(), 0.0f);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Func"))
		{
			pObj->wFunction = (WORD)READ_BITFLAG(bstrData, 0);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Min_X"))
		{
			pObj->vMin.x = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Min_Y"))
		{
			pObj->vMin.y = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Min_Z"))
		{
			pObj->vMin.z = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Max_X"))
		{
			pObj->vMax.x = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Max_Y"))
		{
			pObj->vMax.y = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Max_Z"))
		{
			pObj->vMax.z = READ_FLOAT(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"StateType"))
		{
			pObj->byStateType = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"DefMainState"))
		{
			pObj->byDefMainState = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"DefSubState"))
		{
			pObj->byDefSubState = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcsncmp(pstrDataName->c_str(), L"State", wcslen(L"State")))
		{
			bool bFound = false;
			WCHAR szBuffer[1024] = { 0x00, };

			for (int i = 0; i < DBO_MAX_OBJECT_STATE; i++)
			{
				swprintf(szBuffer, 1024, L"State%d", i);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					READ_STATE(i);
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
		else if (0 == wcsncmp(pstrDataName->c_str(), L"Click_Sound", wcslen(L"Click_Sound")))
		{
			bool bFound = false;

			WCHAR szBuffer[1024] = { 0x00, };
			for (int i = 0; i < DBO_MAX_OBJECT_STATE; i++)
			{
				swprintf(szBuffer, 1024, L"Click_Sound%d", i);

				if (0 == wcscmp(pstrDataName->c_str(), szBuffer))
				{
					READ_STRING(bstrData, pObj->achClickSound[i], _countof(pObj->achClickSound[i]));
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
		else if (0 == wcscmp(pstrDataName->c_str(), L"Boundary_Distance"))
		{
			pObj->byBoundaryDistance = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"ModelName"))
		{
			CheckNegativeInvalid(pstrDataName->c_str(), bstrData);

			READ_STRING(bstrData, pObj->szModelName, _countof(pObj->szModelName));
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Note"))
		{
			// ��ȹ ���� �ʵ�
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Contents_Tblidx"))
		{
			pObj->contentsTblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Object_Direction_Index"))
		{
			pObj->objectDirectionIndex = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"MinQuestId"))
		{
			pObj->minQuestId = READ_WORD(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"MaxQuestId"))
		{
			pObj->maxQuestId = READ_WORD(bstrData, pstrDataName->c_str());
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


sTBLDAT* ObjectTable::FindData(TBLIDX tblidx)
{
	TABLEIT iter = m_mapTableList.find(tblidx);
	if (m_mapTableList.end() == iter) return NULL;

	return (sTBLDAT*)(iter->second);
}

bool ObjectTable::LoadFromBinary(Serializer& serializer, bool bReload)
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
		sOBJECT_TBLDAT* pTableData = new sOBJECT_TBLDAT;
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

bool ObjectTable::SaveToBinary(Serializer& serializer)
{
	serializer.Refresh();

	BYTE byMargin = 1;
	serializer << byMargin;

	TABLE::iterator iter;
	for (iter = Begin(); End() != iter; iter++)
	{
		sOBJECT_TBLDAT* pTableData = (sOBJECT_TBLDAT*)(iter->second);

		pTableData->SaveToBinary(serializer);
	}

	return true;
}