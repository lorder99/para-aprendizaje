﻿#pragma once

#include "NtlPacketCommon.h"
#include "NtlCSArchitecture.h"



//------------------------------------------------------------------
// Packet Enumeration
//------------------------------------------------------------------
enum eOPCODE_AU
{
	AU_OPCODE_BEGIN = 1000,
	#ifdef USE_DBO_TW
	AU_HEARTBEAT,
	#else
	AU_HEARTBEAT = AU_OPCODE_BEGIN,
	#endif
	AU_LOGIN_RES,
	AU_LOGIN_CREATEUSER_RES,
	AU_LOGIN_DISCONNECT_RES,
	#ifdef USE_DBO_TW
	AU_COMMERCIAL_SETTING_NFY,
	#endif
	AU_OPCODE_DUMMY,
	AU_OPCODE_END = AU_OPCODE_DUMMY - 1
};


//------------------------------------------------------------------
//
//------------------------------------------------------------------
const char * NtlGetPacketName_AU(WORD wOpCode);
//------------------------------------------------------------------



#pragma pack(1)

//------------------------------------------------------------------
BEGIN_PROTOCOL(AU_HEARTBEAT)
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(AU_LOGIN_RES)
	WORD		wResultCode;
	WCHAR		awchUserId[NTL_MAX_SIZE_USERID_UNICODE + 1];
	BYTE		abyAuthKey[NTL_MAX_SIZE_AUTH_KEY];
	ACCOUNTID	accountId;
	#ifdef USE_DBO_TW
	BYTE serverID;// Maybe server ID?
	#endif
	SERVERFARMID		lastServerFarmId;
	DWORD				dwAllowedFunctionForDeveloper;
	BYTE				byServerInfoCount;
	sSERVER_INFO		aServerInfo[DBO_MAX_CHARACTER_SERVER_COUNT];
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(AU_LOGIN_CREATEUSER_RES)
	WORD		wResultCode;
	WCHAR		awchUserId[NTL_MAX_SIZE_USERID_UNICODE + 1];
END_PROTOCOL()
//------------------------------------------------------------------
BEGIN_PROTOCOL(AU_LOGIN_DISCONNECT_RES)
END_PROTOCOL()
//------------------------------------------------------------------
#ifdef USE_DBO_TW
BEGIN_PROTOCOL(AU_COMMERCIAL_SETTING_NFY)
END_PROTOCOL()
#endif

#pragma pack()