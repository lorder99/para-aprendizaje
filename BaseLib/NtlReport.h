﻿//***********************************************************************************
//
//	File		:	NtlReport.h
//
//	Begin		:	2009-08-12
//
//	Copyright	:	ⓒ NTL-Inc Co., Ltd
//
//	Author		:	Joon Seok - Hyun
//
//	Desc		:	
//
//***********************************************************************************

#pragma once

#include "NtlSharedType.h"
#include "NTlSharedDef.h"

enum eGM_REPORT_CATEGORY
{
	GM_REPORT_CATEGORY_USERSEARCH,			// 유저 검색
	GM_REPORT_CATEGORY_CHARACTERINFO,		// 기본 정보
	GM_REPORT_CATEGORY_MAIL,					// 메일
	GM_REPORT_CATEGORY_ITEM,					// 아이템	
	GM_REPORT_CATEGORY_QUEST,				// 퀘스트
	GM_REPORT_CATEGORY_PUNISHMENT,			// 제제
	GM_REPORT_CATEGORY_GUILD,				// 유파
	GM_REPORT_CATEGORY_PETITION,				// 진정
	GM_REPORT_CATEGORY_SERVERSTAUTS,			// 서버 상태
	GM_REPORT_CATEGORY_MULTIMAIL,				// 멀티 메일
	GM_REPORT_CATEGORY_DATASETTING,			// 데이터 설정 ( 경험치, EXP )

	GM_REPORT_CATEGORY_COUNT,
	GM_REPORT_CATEGORY_INVALID = 0xFF
};

enum eGM_REPORT_TYPE
{
	GM_REPORT_TYPE_DELETE_CHARACTER,

	GM_REPORT_TYPE_CHANGE_CHARACTER_NAME,
	GM_REPORT_TYPE_CHANGE_CHARACTER_LEVEL,
	GM_REPORT_TYPE_CHANGE_CHARACTER_EXP,
	GM_REPORT_TYPE_CHANGE_RANK_POINT,
	GM_REPORT_TYPE_CHANGE_MODOSA_POINT,
	GM_REPORT_TYPE_CHANGE_HONOR,
	GM_REPORT_TYPE_CHANGE_REVIVAL_SPOT,
	GM_REPORT_TYPE_CHANGE_PRESENT_SPOT,
	GM_REPORT_TYPE_ADD_SKILL,
	GM_REPORT_TYPE_ADD_HTB_SKILL,
	GM_REPORT_TYPE_DELETE_SKILL,
	GM_REPORT_TYPE_DELETE_SKILL_HTB,
	GM_REPORT_TYPE_DELETE_BUF,

	GM_REPORT_TYPE_DELETE_MAIL,
	GM_REPORT_TYPE_DELETE_MAIL_ITEM,
	GM_REPORT_TYPE_DELETE_MAIL_JENNY,
	
	GM_REPORT_TYPE_CHANGE_JENNY,
	GM_REPORT_TYPE_CREATE_ITEM,
	GM_REPORT_TYPE_DELETE_ITEM,
	GM_REPORT_TYPE_DELETE_WAREHOUSE,
	
	GM_REPORT_TYPE_DELETE_QUEST,
	GM_REPORT_TYPE_CREATE_QUEST_ITEM,
	GM_REPORT_TYPE_DELETE_QUEST_ITEM,
	GM_REPORT_TYPE_CHANGE_QUEST_ITEM_COUNT,

	GM_REPORT_TYPE_COUNT_PUNISHMENT,
	GM_REPORT_TYPE_CANCEL_PUNISHMENT,
	
	GM_REPORT_TYPE_CHANGE_GUILD_NAME,
	GM_REPORT_TYPE_CHANGE_GUILD_POINT,
	GM_REPORT_TYPE_CANCEL_GUILD_DELIVERY,
	GM_REPORT_TYPE_INITIALIZE_GUILD_NOTIFY,
	GM_REPORT_TYPE_INITIALIZE_GUILD_EMBLEM,
	GM_REPORT_TYPE_CHANGE_GUILD_ABILITY,
	GM_REPORT_TYPE_DELETE_GUILD_WAREHOUSE_ITEM,
	GM_REPORT_TYPE_CHANGE_GUILD_WAREHOUSE_JENNY,

	GM_REPORT_TYPE_COUNT_PETTION_PROCESS,
	GM_REPORT_TYPE_COUNT_PETTION_PROCESS_STILL,

	GM_REPORT_TYPE_SERVERSTATUS,
	
	GM_REPORT_TYPE_CREATE_MULTIMAIL_ITEM_INDIVIDUAL,
	GM_REPORT_TYPE_CREATE_MULTIMAIL_ITEM_MULTI,
	
	GM_REPORT_TYPE_CHANGE_GAME_EXP,
	GM_REPORT_TYPE_CHANGE_GAME_DROP,	

	GM_REPORT_TYPE_COUNT,
	GM_REPORT_TYPE_INVALID = 255
};

struct sGM_REPORT_CONTENT
{
	eGM_REPORT_CATEGORY category;
	eGM_REPORT_TYPE		reportType;	
	SERVERFARMID		serverFarmID;
	char				accountID[NTL_MAX_SIZE_USERID + 1];
	char				targetID[NTL_MAX_SIZE_USERID + 1];
	char				targetName[NTL_MAX_SIZE_GUILD_NAME + 1]; // 가장 긴 타겟은 유파명
	char				beforeAction[255]; // 따로 define 할 예정
	char				afterAction[255];
	char				objectName[NTL_MAX_LENGTH_REPORT_NAME + 1];
	TBLIDX				tblIdx;
	int					serial;
	int					result;
};

extern const sGM_REPORT_CONTENT g_aGM_REPORT_TYPE[GM_REPORT_TYPE_COUNT];

BYTE GetCategory(eGM_REPORT_TYPE);