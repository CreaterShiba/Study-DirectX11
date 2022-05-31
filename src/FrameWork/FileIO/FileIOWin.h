
#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		FileIOWin.h
	@brief		ファイル読み込み(Windows)
	@author		shiba
	@date		2013/2/2 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "FileIOBase.h"

/*==========================================================================*/
/*!
	@class	CFileIOWin
	@brief	ファイル読み込み(Windows)
*/
/*==========================================================================*/
class CFileIOWin : public CFileIOBase
{
public:
	CFileIOWin();
	~CFileIOWin();

	//@brief 初期化
	virtual bool Init();

	//@brief ファイルオープン
	virtual bool Open( const TCHAR* pFileName, const E_ACCESS_TYPE eType );

	//@brief ファイルオープン(非同期)
	virtual bool OpenAsync( const TCHAR* pFileName, const E_ACCESS_TYPE eType );

	//@brief ファイルクローズ
	virtual bool Close();

	//@brief ファイルリード
	virtual u32	Read( void* pBuf, const u32 Size );

	//@brief ファイル書き込み
	virtual u32	 Write( const void* const pBuf, const u32 Size );

	//@brief オープンできたか
	virtual bool IsExecuting();

private:
	HANDLE m_hFileHandle;	//!< ファイルハンドル
	HANDLE m_hEventHandle;	//!< イベントハンドル
	OVERLAPPED m_sOverLap;	//!< 非同期読み込みに必要な構造体
	bool m_bAsync;
};

//EOF
