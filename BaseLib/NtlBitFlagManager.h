//***********************************************************************************
//
//	File		:	NtlBitFlagManager.h
//
//	Begin		:	2007-01-22
//
//	Copyright	:	�� NTL-Inc Co., Ltd
//
//	Author		:	
//
//	Desc		:	
//
//***********************************************************************************

#pragma once

#include <windows.h>

class CNtlBitFlagManager
{
public:

	CNtlBitFlagManager(void);

	virtual ~CNtlBitFlagManager(void);


public:

	bool							Create(DWORD dwMaxFlagCount);

	bool							Create(void* pvFlag, DWORD dwMaxFlagCount);

	void							Destroy();


protected:

	void							Init();

public:

	bool							Set(DWORD dwIndex);

	bool							Unset(DWORD dwIndex);

	bool							IsSet(DWORD dwIndex);

	void							Not();

	bool							Replace(void* pvNewFlag, DWORD dwMaxFlagCount);

public:

	BYTE*							GetRawData() { return m_pabyFlag; }

	DWORD							GetBytesUsed() { return m_dwBytesUsed; }

	DWORD							GetMaxFlagCount() { return m_dwMaxFlagCount; }


private:

	BYTE*							m_pabyFlag;
	DWORD							m_dwBytesUsed;

	DWORD							m_dwMaxFlagCount;
};