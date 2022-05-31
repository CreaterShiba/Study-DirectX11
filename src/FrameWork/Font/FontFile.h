
#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		FontFile.h
	@brief		フォント管理
	@author		shiba
	@date		2013/1/19 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Utility/UtilDefine.h"

/*==========================================================================*/
/*!
	@class	CFontFile
	@brief	フォントファイル
*/
/*==========================================================================*/
class CFontFile
{
public:
	//@brief コンストラクタ
	CFontFile();
	//@brief デストラクタ
	~CFontFile();

	//@brief 初期化
	bool Init();

	//@brief システムフォントロード
	bool LoadSystemFont( const TCHAR* pcFontName, const s32 nXSize, const s32 nYSize );

	//@brief フォントファイルロード
	bool LoadFont( const TCHAR* pcFileName, TCHAR* pcFontname, const s32 nXSize, const s32 nYSize );

public:
	TCHAR m_FileName[ 32 ];	//!< ファイル名
	HFONT m_hFont;			//!< 倫理フォントデータ
	HFONT m_hOldFont;		//!< ローカルにもっていけるかな？
	HDC	  m_hHdc;			//!< ハンドルかな？
	s32	  m_nGrad;			//!< グラデ
	s32	  m_nGradFlag;		//!< グラデ
	TEXTMETRIC m_tm;		//!< よくわかんないな

private:
	//フォントタイプ
	enum E_FONTFILE_TYPE
	{
		eFontfile_Type_None,
		eFontfile_Type_System,	//!< システムフォント
		eFontfile_Type_File,	//!< オリジナルフォント
	};
	E_FONTFILE_TYPE m_eFontType;	//!< フォントタイプ
};

//EOF

