/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		FontFile.cpp
	@brief		フォント管理
	@author		shiba
	@date		2013/1/19 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "FontFile.h"

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CFontFile::CFontFile()
: m_hFont(  )
, m_hOldFont(  )
, m_hHdc(  )
, m_nGrad( 0 )
, m_nGradFlag( 0 )
, m_eFontType( eFontfile_Type_None )
{
	ZeroMemory( m_FileName, sizeof( m_FileName ) );
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CFontFile::~CFontFile()
{
	//デバイスコンテキストとフォントハンドルは開放
	SelectObject( m_hHdc, m_hOldFont );
	ReleaseDC(nullptr, m_hHdc );

	if( m_eFontType == eFontfile_Type_File )
	{
		RemoveFontResourceEx( m_FileName, FR_PRIVATE, nullptr);
	}
}

//---------------------------------------------------------------------------
/*!
	@brief	初期化
*/
//---------------------------------------------------------------------------
bool CFontFile::Init()
{
	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief		システムフォントロード
	@brief		フォントファイルロード
	@param[in]	フォント名
	@param[in]	フォント一個あたりの横サイズ
	@param[in]	フォント一個あたりの縦サイズ
*/
//---------------------------------------------------------------------------
bool CFontFile::LoadSystemFont( const TCHAR* pcFontName, const s32 nXSize, const s32 nYSize )
{
	//フォントの作成処理
	LOGFONT lf = 
	{
		nXSize, 0, 0, 0, nYSize, 0, 0, 0,
		SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY, DEFAULT_PITCH | FF_MODERN, 0
	};
	memcpy( lf.lfFaceName, pcFontName, LF_FACESIZE );

	m_hOldFont = CreateFontIndirect( &lf );
	if( !m_hOldFont )
	{
		return false;
	}

	//現ウィンドウに適用
	//デバイスにフォントを持たせないとGetGlyphOutline関数はエラー
	m_hHdc = GetDC(nullptr);
	m_hFont = (HFONT)SelectObject( m_hHdc, m_hOldFont );

	// GGO_GRAY2_BITMAP or GGO_GRAY4_BITMAP or GGO_GRAY8_BITMAP
	m_nGradFlag = GGO_GRAY4_BITMAP; 

	switch( m_nGradFlag ) 
	{
	case GGO_GRAY2_BITMAP: 
		m_nGrad =  4; break;
	case GGO_GRAY4_BITMAP: 
		m_nGrad = 16; break;
	case GGO_GRAY8_BITMAP:
		m_nGrad = 64; break;
	default:
		break;
	}
	if (m_nGrad == 0)
	{
		return false;
	}

	GetTextMetrics( m_hHdc, &m_tm );

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief		フォントファイルロード
	@param[in]	フォントファイル名
	@param[in]	フォント名
	@param[in]	フォント一個あたりの横サイズ
	@param[in]	フォント一個あたりの縦サイズ
*/
//---------------------------------------------------------------------------
bool CFontFile::LoadFont( const TCHAR* pcFileName, TCHAR* pcFontname, const s32 nXSize, const s32 nYSize )
{
	//システム上に指定フォントを追加
	s32 nAddFontnum = AddFontResourceEx( pcFileName, FR_PRIVATE, nullptr);

	if( nAddFontnum > 0 )
	{
		NS_Report(_T("LoadFont : %s ::%d::\n"), pcFileName, nAddFontnum );
		memcpy( m_FileName, pcFileName, sizeof( pcFileName ) );

		bool bRet = LoadSystemFont( pcFontname, nXSize, nYSize );
		if( bRet )
		{
			m_eFontType = eFontfile_Type_File;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

//EOF
