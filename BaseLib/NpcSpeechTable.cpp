#include "StdAfx.h"
#include "NpcSpeechTable.h"
#include "Serializer.h"

WCHAR* NpcSpeechTable::m_pwszSheetList[] =
{
	L"Table_Data_KOR",
	NULL
};

NpcSpeechTable::NpcSpeechTable(void)
{
	Init();
}

NpcSpeechTable::~NpcSpeechTable(void)
{
	Destroy();
}

bool NpcSpeechTable::Create(DWORD dwCodePage)
{
	return Table::Create(dwCodePage);
}

void NpcSpeechTable::Destroy()
{
	Table::Destroy();

	for (SPEECH_GROUP_IT it = m_speechGroupList.begin(); it != m_speechGroupList.end(); it++)
	{
		delete it->second;
	}
}

void NpcSpeechTable::Init()
{
}

void* NpcSpeechTable::AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sNPC_SPEECH_TBLDAT* pNpcSpeech = new sNPC_SPEECH_TBLDAT;
		if (NULL == pNpcSpeech)
			return NULL;

		CPINFO cpInfo;
		if (false == GetCPInfo(dwCodePage, &cpInfo))
		{
			delete pNpcSpeech;
			return NULL;
		}

		m_dwCodePage = dwCodePage;
		return pNpcSpeech;
	}

	return NULL;
}

bool NpcSpeechTable::DeallocNewTable(void* pvTable, WCHAR* pwszSheetName)
{
	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sNPC_SPEECH_TBLDAT* pNpcSpeech = (sNPC_SPEECH_TBLDAT*)pvTable;
		if (FALSE != IsBadReadPtr(pNpcSpeech, sizeof(*pNpcSpeech)))
			return false;

		delete pNpcSpeech;

		return true;
	}

	return false;
}

bool NpcSpeechTable::AddTable(void * pvTable, bool bReload)
{
	UNREFERENCED_PARAMETER(bReload);

	sNPC_SPEECH_TBLDAT* pTbldat = (sNPC_SPEECH_TBLDAT*)pvTable;

	if (false == m_mapTableList.insert(std::pair<TBLIDX, sTBLDAT*>(pTbldat->tblidx, pTbldat)).second)
	{
		Table::CallErrorCallbackFunction(L"[File] : %s\r\n Table Tblidx[%u] is Duplicated ", m_wszXmlFileName, pTbldat->tblidx);
		_ASSERTE(0);
		return false;
	}


	if (INVALID_NPC_SPEECH_TYPE != pTbldat->bySpeechType)
	{
		if (false == AddSpeechGroup(pTbldat))
		{
			_ASSERTE("AddSpeechGroup Fail");
			return false;
		}
	}

	return true;
}

bool NpcSpeechTable::SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData)
{

	if (0 == wcscmp(pwszSheetName, L"Table_Data_KOR"))
	{
		sNPC_SPEECH_TBLDAT * pNpcSpeech = (sNPC_SPEECH_TBLDAT*)pvTable;

		if (0 == wcscmp(pstrDataName->c_str(), L"Tblidx"))
		{
			pNpcSpeech->tblidx = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Dialog_Group"))
		{
			pNpcSpeech->dwDialogGroup = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Dialog_Type"))
		{
			READ_STRING(bstrData, pNpcSpeech->szDialogType, _countof(pNpcSpeech->szDialogType));

			if (0 == _strnicmp(pNpcSpeech->szDialogType, "Fainting", strlen("Fainting")))
			{
				pNpcSpeech->bySpeechType = NPC_SPEECH_TYPE_FAINT;
			}
			else if (0 == _strnicmp(pNpcSpeech->szDialogType, "Idle", strlen("Idle")))
			{
				pNpcSpeech->bySpeechType = NPC_SPEECH_TYPE_IDLE;
			}
			else if (0 == _strnicmp(pNpcSpeech->szDialogType, "Fighting", strlen("Fighting")))
			{
				pNpcSpeech->bySpeechType = NPC_SPEECH_TYPE_FIGHTING;
			}
			else
			{
				pNpcSpeech->bySpeechType = INVALID_NPC_SPEECH_TYPE;
			}
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Rate"))
		{
			pNpcSpeech->byRate = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Text_Index"))
		{
			pNpcSpeech->textIndex = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Ballon_Type"))
		{
			pNpcSpeech->byBallonType = READ_BYTE(bstrData, pstrDataName->c_str());
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Display_Time"))
		{
			pNpcSpeech->dwDisplayTime = READ_DWORD(bstrData);
		}
		else if (0 == wcscmp(pstrDataName->c_str(), L"Note"))
		{
			READ_STRING(bstrData, pNpcSpeech->szNote, _countof(pNpcSpeech->szNote));
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

sTBLDAT* NpcSpeechTable::FindData(TBLIDX tblidx)
{
	if (0 == tblidx)
		return NULL;

	TABLEIT iter;
	iter = m_mapTableList.find(tblidx);
	if (End() == iter)
		return NULL;

	return (sTBLDAT*)(iter->second);
}

bool NpcSpeechTable::LoadFromBinary(Serializer& serializer, bool bReload)
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
		sNPC_SPEECH_TBLDAT* pTableData = new sNPC_SPEECH_TBLDAT;
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

bool NpcSpeechTable::SaveToBinary(Serializer& serializer)
{
	serializer.Refresh();

	BYTE byMargin = 1;
	serializer << byMargin;

	TABLE::iterator iter;
	for (iter = Begin(); End() != iter; iter++)
	{
		sNPC_SPEECH_TBLDAT* pTableData = (sNPC_SPEECH_TBLDAT*)(iter->second);

		pTableData->SaveToBinary(serializer);
	}

	return true;
}

sNPC_SPEECH_GROUP * NpcSpeechTable::FindSpeechGroup(DWORD dwSpeechGroup)
{
	if (INVALID_DWORD == dwSpeechGroup)
	{
		return NULL;
	}

	SPEECH_GROUP_IT it = m_speechGroupList.find(dwSpeechGroup);
	if (it == m_speechGroupList.end())
	{
		return NULL;
	}

	return it->second;
}

bool NpcSpeechTable::AddSpeechGroup(sNPC_SPEECH_TBLDAT * pNpcSpeechTbldat)
{
	sNPC_SPEECH_GROUP * pSpeechGroup = FindSpeechGroup(pNpcSpeechTbldat->dwDialogGroup);
	if (NULL == pSpeechGroup)
	{
		pSpeechGroup = new sNPC_SPEECH_GROUP;
		if (NULL == pSpeechGroup)
		{
			_ASSERTE(0);
			return false;
		}

		if (false == m_speechGroupList.insert(SPEECH_GROUP_VAL(pNpcSpeechTbldat->dwDialogGroup, pSpeechGroup)).second)
		{
			_ASSERTE(0);
			return false;
		}
	}


	if (false == pSpeechGroup->SetSpeechData((eNPC_SPEECH_TYPE)pNpcSpeechTbldat->bySpeechType,
		pNpcSpeechTbldat->byRate,
		pNpcSpeechTbldat->tblidx))
	{
		_ASSERTE(0);
		return false;
	}


	return true;
}

bool sNPC_SPEECH_GROUP::SetSpeechData(eNPC_SPEECH_TYPE eSpeechType, BYTE byProbValue, TBLIDX speechTblidx)
{
	if (eSpeechType >= MAX_NPC_SPEECH_TYPE)
	{
		_ASSERTE("Speech Type exceed MAX_NPC_SPEECH_TYPE");
		return false;
	}

	sSPEECH_GROUP_DATA * pSpeechGroupData = &asSpeechGroupDataData[eSpeechType];

	BYTE byLastProb = 0;
	for (sSPEECH_GROUP_DATA::LIST_IT it = pSpeechGroupData->speechDataList.begin()
		; pSpeechGroupData->speechDataList.end() != it
		; ++it)
	{
		byLastProb = (*it).bySpeechProb - byLastProb;
	}

	sSPEECH_DATA speechData;
	speechData.bySpeechProb = byLastProb + byProbValue;
	speechData.speechTblidx = speechTblidx;

	if (speechData.bySpeechProb > DBO_MAX_SPEECH_RANDOM_VALUE)
	{
		_ASSERTE("Speech Prob exceed DBO_MAX_SPEECH_RANDOM_VALUE");
		return false;
	}

	pSpeechGroupData->byNonSpeechProb = 100/*100%*/ - speechData.bySpeechProb;

	pSpeechGroupData->speechDataList.push_back(speechData);

	return true;
}

TBLIDX sNPC_SPEECH_GROUP::GetSpeechData(eNPC_SPEECH_TYPE eSpeechType, BYTE byProbValue)
{
	if (byProbValue < DBO_MIN_SPEECH_RANDOM_VALUE || byProbValue > DBO_MAX_SPEECH_RANDOM_VALUE)
	{
		_ASSERTE("byProbValue out of range[ DBO_MIN_SPEECH_RANDOM_VALUE <= byProvValue <= DBO_MAX_SPEECH_RANDOM_VALUE ]");
		return INVALID_TBLIDX;
	}


	if (eSpeechType >= MAX_NPC_SPEECH_TYPE)
	{
		_ASSERTE("Speech Type exceed MAX_NPC_SPEECH_TYPE");
		return INVALID_TBLIDX;
	}

	sSPEECH_GROUP_DATA * pSpeechGroupData = &asSpeechGroupDataData[eSpeechType];

	// check non speech
	if (byProbValue > pSpeechGroupData->byNonSpeechProb)
	{
		return INVALID_TBLIDX;
	}

	// select speech
	for (sSPEECH_GROUP_DATA::LIST_IT it = pSpeechGroupData->speechDataList.begin(); it != pSpeechGroupData->speechDataList.end(); it++)
	{
		if (byProbValue <= (*it).bySpeechProb)
		{
			return (*it).speechTblidx;
		}
	}

	return INVALID_TBLIDX;
}

bool sNPC_SPEECH_GROUP::HasSpeechData(eNPC_SPEECH_TYPE eSpeechType)
{
	if (eSpeechType >= MAX_NPC_SPEECH_TYPE)
	{
		_ASSERTE("Speech Type exceed MAX_NPC_SPEECH_TYPE");
		return false;
	}

	return !(asSpeechGroupDataData[eSpeechType].speechDataList.empty());
}
