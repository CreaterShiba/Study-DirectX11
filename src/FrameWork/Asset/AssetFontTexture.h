#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		AssetFontTexture.h
	@brief		フォント用テクスチャクラス
	@authqor	shiba
	@date		2013/1/19 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Asset/AssetTexture.h"
#include "FrameWork/Font/FontDataType.h"

class CFontFile;
/*==========================================================================*/
/*!
	@class	CAssetFontTexture
	@brief	フォントテクスチャ
*/
/*==========================================================================*/
class CAssetFontTexture : public CAssetTexture
{
public:
	//@brief コンストラクタ
	CAssetFontTexture();

	//@brief デストラクタ
	~CAssetFontTexture();

	//@brief 初期化
	bool Init();

	// @brief フォントテクスチャ作成
	bool LoadFontTexture();

	//@brief フォントデータ満タンじゃないか
	const bool IsFullFont() const; 

	//@brief フォントデータの取得
	const CFontOneData* GetFontData( const u16 ucCode ) const;

	//@brief フォントデータの書き込み
	const CFontOneData& LoadGryphToTexture( const u16 unCode, const CFontFile* const pcFont );

private:
	ID3D11Texture2D* m_pTexture;
	std::vector< CFontOneData > m_pcaFontData;	//!< フォントデータ
};

//EOF