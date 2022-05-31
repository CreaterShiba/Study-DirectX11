
#pragma once
#include "FontDataType.h"

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UtilFont.h
	@brief		フォント周りインターフェイス
	@author		shiba
	@date		2012/9/14 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
class CUtilFont
{
public:
	CUtilFont();
    ~CUtilFont();

#if false
	union
	{
		//ちょっとビットフラグ試してみる
		u32	m_unFontRenderTypes;
		struct
		{
			u32	Normal	:	1;	//ノーマル
			u32	Alpha	:	1;	//アルファ入り
		} m_FontRenderTypeBit;
	};
#endif

public:
	
	//!< 2D文字列描画
	static bool RenderText2D
		(
			const E_FONTDATA_FILE eType,	//フォントタイプ
			const TCHAR* strChar,			//文字列
			//const u32	unBitType,			//描画手法
			//const CUtilColor& cColor,		//カラー
			const NSVector2& vPos			//2D座標
		);

	//!< 3D文字列描画
	static bool RenderText3D
		(
			const E_FONTDATA_FILE eType,	//フォントタイプ
			const TCHAR* strChar,		//文字列
			//const u32	unBitType,		//描画手法
			//const CUtilColor& cColor,	//カラー
			const NSVector3& vPos		//3D座標
		);
};

//EOF