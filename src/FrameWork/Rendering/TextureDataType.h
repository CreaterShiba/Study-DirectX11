#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		TextureDataType.h
	@brief		テクスチャデータ形式宣言
	@author		shiba
	@date		2012/9/29 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

// @brief 読み込み時のテクスチャ指定データ
// @todo どこかと統合できるかな？
class SInitTextureParam
{
public:
	SInitTextureParam() :
		m_unSizeX(0),
		m_unSizeY(0),
		m_TexFormat(DXGI_FORMAT_UNKNOWN),
		bTGA(false)
	{
		memset(strTexName, 0, sizeof(strTexName));
	}
public:
	//!< テクスチャファイル名
	TCHAR strTexName[256];

	//!< 横サイズ
	u32 m_unSizeX;

	//!< 縦サイズ
	u32 m_unSizeY;

	//!< フォーマット
	DXGI_FORMAT m_TexFormat;

	//!< TGA or PNG、ファイル内に埋め込み予定
	bool bTGA;
};

//EOF
