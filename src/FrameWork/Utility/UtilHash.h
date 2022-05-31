#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UtilHash.h
	@brief		ハッシュ汎用クラス
	@author		shiba
	@date		2012/9/11 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
//#include "FrameWork/Utility/UtilDefine.h"
#include <stdio.h>
#include <stdint.h>
#include "Tchar.h"

/*==========================================================================*/
/*!
@class	CUtilHash
@brief	ハッシュユーティリティー
*/
/*==========================================================================*/
class CUtilHash
{
	CUtilHash() {}
	~CUtilHash() {}

public:
	//---------------------------------------------------------------------------
	/*!
	@brief		ファイル名からハッシュ番号を取得（簡易版)
	@param[in]	name	ファイル名
	*/
	//---------------------------------------------------------------------------
	static u32 calcNameHash(const TCHAR* name)
	{
#if 0
		u32 nameHash = 0;
		s32 nLen = 0;
#if defined( UNICODE )
		nLen = wcslen(name);
#else
		nLen = strlen(name);
#endif

		for (s32 i = 0; i < nLen; i++)
		{
			nameHash += name[i];
		}
		return nameHash;
#else
		return fnv_1_hash_32(name, wcslen(name));
#endif
	}

private:
	//http://qiita.com/yamasaki-masahide/items/d478dd111690bc84784a
	//http://d.hatena.ne.jp/jonosuke/20100406/p1
	/**
	* FNV Constants
	*/
	static const uint32_t FNV_OFFSET_BASIS_32 = 2166136261U;
	static const uint64_t FNV_OFFSET_BASIS_64 = 14695981039346656037U;

	static const uint32_t FNV_PRIME_32 = 16777619U;
	static const uint64_t FNV_PRIME_64 = 1099511628211LLU;

	/**
	* FNV Hash Algorithm
	*/
	static uint32_t fnv_1_hash_32(const TCHAR *bytes, size_t length)
	{
		uint32_t hash;
		size_t i;

		hash = FNV_OFFSET_BASIS_32;
		for (i = 0; i < length; ++i) {
			hash = (FNV_PRIME_32 * hash) ^ (bytes[i]);
		}

		return hash;
	}
	//static uint64_t fnv_1_hash_64(uint8_t *bytes, size_t length)
	//{
	//	uint64_t hash;
	//	size_t i;

	//	hash = FNV_OFFSET_BASIS_64;
	//	for (i = 0; i < length; ++i) {
	//		hash = (FNV_PRIME_64 * hash) ^ (bytes[i]);
	//	}

	//	return hash;
	//}
};

//EOF