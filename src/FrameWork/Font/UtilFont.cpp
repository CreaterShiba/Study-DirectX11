
/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UtilFont.cpp
	@brief		フォント周りインターフェイス
	@author		shiba
	@date		2012/9/14 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/

#include "UtilFont.h"
#include "FrameWork/Font/FontManager.h"

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CUtilFont::CUtilFont()
{
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CUtilFont::~CUtilFont()
{
}

//---------------------------------------------------------------------------
/*!
	@brief		2D文字列描画
	@param[in]	フォントタイプ
	@param[in]	文字列
	@param[in]	描画手法
	@param[in]	文字色
	@param[in]	2D座標
*/
//---------------------------------------------------------------------------
bool CUtilFont::RenderText2D
(
	const E_FONTDATA_FILE eType,
	const TCHAR *strChar,
	const NSVector2 &vPos
)
{
	CFontManager* pcFontM = CFontManager::GetInstance();
	if (pcFontM != nullptr)
	{
		pcFontM->DrawText2D(strChar, vPos, eType);
	}

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief		3D文字列描画
	@param[in]	フォントタイプ
	@param[in]	文字列
	@param[in]	描画手法
	@param[in]	文字色
	@param[in]	3D座標
*/
//---------------------------------------------------------------------------
bool CUtilFont::RenderText3D
( 
	const E_FONTDATA_FILE eType,
	const TCHAR *strChar,
	const NSVector3 &vPos
)
{
	NS_UNUSED(eType);
	NS_UNUSED(strChar);
	NS_UNUSED(vPos);
	return true;
}

//EOF
