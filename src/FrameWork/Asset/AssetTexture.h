#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
@file		AssetTexture.h
@brief		テクスチャ管理
@author		shiba
@date		2013/4/28 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "AssetBase.h"
#include "FrameWork/Rendering/TextureDataType.h"

/*==========================================================================*/
/*!
	@class	CAssetTexture
	@brief	テクスチャー
*/
/*==========================================================================*/
class CAssetTexture : public CAssetBase
{
	//コピー禁止
	void operator =(const CAssetTexture& src) { NS_UNUSED(src);  }
	CAssetTexture(const CAssetTexture& src);

public:
	// @brief コンストラクタ
	CAssetTexture();

	// @brief デストラクタ
	virtual ~CAssetTexture();

	// @brief テクスチャリソースビュー設定
	inline void SetTextureRevi(ID3D11ShaderResourceView* pRevi);

	// @brief テクスチャリソースビュー獲得
	inline ID3D11ShaderResourceView* GetTextureRevi() const;

protected:
	//!< シェーダリソース 
	ID3D11ShaderResourceView* m_pTextureReVi;

	//!< テクスチャデータ
	SInitTextureParam m_cTextureParam;
};

void CAssetTexture::SetTextureRevi(ID3D11ShaderResourceView* pRevi)
{
	m_pTextureReVi = pRevi;
}

ID3D11ShaderResourceView* CAssetTexture::GetTextureRevi() const
{
	return m_pTextureReVi;
}

//EOF