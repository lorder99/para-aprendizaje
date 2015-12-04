#ifndef _TABLE
#define _TABLE

#pragma once

#include "NtlSharedType.h"
#include "Serializer.h"

#include <comutil.h>
#include <map>

#include "Base.h"
#include "Def.h"
#include "Logger.h"

enum eLOADING_METHOD
{
	LOADING_METHOD_XML = 0,
	LOADING_METHOD_BINARY,
	LOADING_METHOD_SECURED_BINARY,

	LOADING_METHOD_INVALID = 0xFFui8
};

enum eCONSTANT
{
	TABLE_XML_FILE_NAME_MAX_LENGTH = 128
};

class Serializer;
class XMLDoc;

#pragma pack(push, 4)
struct sTBLDAT
{
public:
	sTBLDAT()
		:tblidx(INVALID_TBLIDX) {}
	virtual ~sTBLDAT() {}
	TBLIDX				tblidx;

	bool LoadFromBinary(Serializer& serializer)
	{
		if (serializer.GetDataSize() < GetDataSize())
		{
			return false;
		}

		serializer.Out(GetDataAddress(), GetDataSize());
		return true;
	}

	bool LoadFromChunk(Serializer& serializer)
	{
		if (serializer.GetDataSize() < GetDataSize())
		{
			return false;
		}

		serializer.Out(GetDataAddress(), GetDataSize());
		return true;
	}

	void SaveToBinary(Serializer& serializer)
	{
		serializer.In(GetDataAddress(), GetDataSize());
	}

	void* GetDataAddress()
	{
		return (char*)this + sizeof(void*);
	}

	virtual int GetDataSize()
	{
		return sizeof(*this) - sizeof(void*);
	}
};
#pragma pack(pop)

typedef void(*Dbo_TableErrorCallback)(char* pszMessage, void* pvArg);

class Table
{
public:
	Table(void);
	virtual ~Table(void);

	bool Create(DWORD dwCodePage);
	virtual void Destroy();

	typedef std::map<TBLIDX, sTBLDAT *> TABLE;
	typedef TABLE::iterator TABLEIT;
	typedef TABLE::value_type TABLEVAL;

	TABLEIT	Begin() { return m_mapTableList.begin(); }
	TABLEIT	End() { return m_mapTableList.end(); }
	virtual sTBLDAT* FindData(TBLIDX tblidx) { (void)tblidx; return NULL; }
	size_t GetNumberOfTables() { return m_mapTableList.size(); }


	void Reset();
	bool LoadFromXml(char* pszFileName, bool bReload);
	bool LoadFromXml(WCHAR* pwszFileName, bool bReload);
	bool LoadFromChunk(Serializer& serializer, bool bReload);
	virtual bool LoadFromBinary(Serializer& serializer, bool bReload) = 0;
	virtual bool SaveToBinary(Serializer& serializer) = 0;
	DWORD HexToDec(WCHAR* pwszHexString);

	static void SetErrorCallbackFunction(Dbo_TableErrorCallback pfnErrorCallback, void* pvErrorCallbackArg = NULL);
	static void CallErrorCallbackFunction(char* pszFormatString, ...);
	static void CallErrorCallbackFunction(WCHAR* pwszFormatString, ...);
	WCHAR* GetWszXmlFileName() { return m_wszXmlFileName; }

protected:
	void Init();
	virtual WCHAR** GetSheetListInWChar() = 0;
	virtual void* AllocNewTable(WCHAR* pwszSheetName, DWORD dwCodePage) = 0;
	virtual bool DeallocNewTable(void* pvTable, WCHAR* pwszSheetName) = 0;
	virtual bool AddTable(void* pvTable, bool bReload) = 0;
	virtual bool SetTableData(void* pvTable, WCHAR* pwszSheetName, std::wstring* pstrDataName, BSTR bstrData) = 0;
	virtual bool InitializeFromXmlDoc(XMLDoc* pXmlDoc, WCHAR* pwszFileName, bool bReload);
	
	bool CheckInvalidValue(BSTR bstr) { return bstr[0] == L'@' ? true : false; }
	char READ_CHAR(BSTR bstr, const WCHAR* pwszFieldName, BYTE chInvalidValue = 0);
	BYTE READ_BYTE(BSTR bstr, const WCHAR* pwszFieldName, BYTE byInvalidValue = INVALID_BYTE);
	WORD READ_WORD(BSTR bstr, const WCHAR* pwszFieldName, WORD wInvalidValue = INVALID_WORD);
	DWORD READ_DWORD(BSTR bstr, DWORD dwInvalidValue = INVALID_DWORD){ return CheckInvalidValue(bstr) ? dwInvalidValue : (DWORD)(_wtoi64(bstr)); }
	TBLIDX READ_TBLIDX(BSTR bstr, DWORD dwInvalidValue = INVALID_TBLIDX){ return CheckInvalidValue(bstr) ? dwInvalidValue : (DWORD)(_wtoi64(bstr)); }
	FLOAT READ_FLOAT(BSTR bstr, const WCHAR* pwszFieldName, float fInvalidValue = INVALID_FLOAT);
	BSTR READ_STR(BSTR bstr, const char * lpszInvalidValue = "") { return CheckInvalidValue(bstr) ? (BSTR)lpszInvalidValue : bstr; }
	bool READ_STR(std::string & rDest, BSTR bstr, const char * lpszInvalidValue = "");
	bool READ_STR(std::wstring & rDest, BSTR bstr, const WCHAR * lpwszInvalidValue = L"");
	bool READ_STRING(BSTR bstr, char* pszBuffer, DWORD dwBufferLength, const char* pszInvalidValue = "");
	bool READ_STRINGW(BSTR bstr, WCHAR* pwszBuffer, DWORD dwBufferLength, const WCHAR* pwszInvalidValue = L"");
	bool READ_BOOL(BSTR bstr, const WCHAR* pwszFieldName, bool bInvalidlValue = false);
	DWORD READ_BITFLAG(BSTR bstr, DWORD dwInvalidValue = 0) { return CheckInvalidValue(bstr) ? dwInvalidValue : HexToDec(bstr); }
	void CheckNegativeInvalid(const wchar_t* pwszFormatString, BSTR bstr); // @�� ������ �ȵǴ� �ʵ�� Assert�� �ѷ��ش�.

	TABLE m_mapTableList;
	DWORD m_dwCodePage;

	// This is only for displaying a message, so it can't be used for the other purposes.
	WCHAR m_wszXmlFileName[TABLE_XML_FILE_NAME_MAX_LENGTH + 1];
	static Dbo_TableErrorCallback m_pfnErrorCallback;
	static void* m_pvErrorCallbackArg;
};


#endif