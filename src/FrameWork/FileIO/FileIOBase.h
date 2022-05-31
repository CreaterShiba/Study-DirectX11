
#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		FileIOBase.h
	@brief		ファイルIOベースクラス
	@author		shiba
	@date		2012/10/14 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Utility/UtilDefine.h"

/*==========================================================================*/
/*!
	@class	CFileIOBase
	@brief	ファイルIOベースクラス
*/
/*==========================================================================*/
class CFileIOBase
{
public:
	enum E_ACCESS_TYPE
	{
		eFile_Access_None,
		eFile_Access_Read,
		eFile_Access_Write,
		eFile_Access_Max,
	};

public:
	CFileIOBase()
	: m_FileSize( 0 )
	, m_bLoadFile( false )
	, m_eAccessType( eFile_Access_None )
	{}
	virtual ~CFileIOBase(){};


public:
	//@brief 初期化
	virtual bool Init() = 0;

	//@brief ファイルオープン
	virtual bool Open( const TCHAR* pFileName, const E_ACCESS_TYPE eType ) = 0;

	//@brief ファイルオープン(非同期)
	virtual bool OpenAsync( const TCHAR* pFileName, const E_ACCESS_TYPE eType ) = 0;

	//@brief ファイルクローズ
	virtual bool Close(void)=0;

	//@brief ファイルリード
	virtual u32	 Read( void* pBuf, const u32 Size )=0;

	//@brief ファイル書き込み
	virtual u32	 Write( const void* const pBuf, const u32 Size )=0;

	//@brief オープンできたか
	virtual bool IsExecuting()
	{
		return m_bLoadFile;
	}

	//@brief ファイルサイズ獲得
	inline u32 GetSize() const;

protected:
	u32	m_FileSize;					//!< ファイルサイズ
	bool m_bLoadFile;				//!< ファイルロード済み
	E_ACCESS_TYPE m_eAccessType;	//!< ファイルアクセスタイプ
	TCHAR m_strFileName[256];		//!< ファイル名
};

u32 CFileIOBase::GetSize() const
{
	return m_FileSize;
}

//EOF
