#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		Material.h
	@brief		マテリアルクラス
	@author		shiba
	@date		2012/9/22 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Rendering/ModelDataType.h"

class CShaderBase;
/*==========================================================================*/
/*!
	@class	CMaterial
	@brief	マテリアル情報クラス
	@note	コピー対象になってるので、変数を追加した際は気をつける
*/
/*==========================================================================*/
class CMaterial
{
public:
	// @brief コンストラクタ
	CMaterial();

	// @brief デストラクタ
	~CMaterial();

	// @brief 初期化
	bool Init();

	// @brief 描画パイプラインに適用
	void OnMaterial();

	// @brief 描画パイプラインもどす
	void OffMaterial();

	// @brief シェーダ設定
	inline void SetShader(CShaderBase* pcShader);

	// @brief シェーダ獲得
	inline CShaderBase* GetShader() const;

	// @brief テクスチャIDの設定
	inline void SetDiffuseTextureID(const u32 unTexID);
	inline u32 GetDiffuseTextureID() const;

	inline void SetNormalTextureID(const u32 unTexID);
	inline u32 GetNormalTextureID() const;

	// @brief 透明度ありか
	inline bool IsAlphaBlend() const;

private:
	//!< マテリアル情報(描画情報)
	SMaterialData m_sMaterialData;

	//!< 各種テクスチャのAssetID
	u32 m_unDiffuseTextureID;
	u32 m_unNormalTextureID;

	//!< 描画シェーダー
	CShaderBase* m_pcShader;

public:

#if defined( _DEBUG )
	//!< デバッグ用ビュー
	ID3D11ShaderResourceView* m_pcDebugView;
	void SetDebugView(ID3D11ShaderResourceView* pcResource)
	{
		m_pcDebugView = pcResource;
	}
#endif

};

//---------------------------------------------------------------------------
/*!
	@brief	シェーダ設定
	@param[in]	pcShader	シェーダ
*/
//---------------------------------------------------------------------------
void CMaterial::SetShader(CShaderBase* pcShader)
{
	m_pcShader = pcShader;
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダ獲得
*/
//---------------------------------------------------------------------------
CShaderBase* CMaterial::GetShader() const
{
	return m_pcShader;
}

//---------------------------------------------------------------------------
/*!
	@brief	テクスチャID設定
*/
//---------------------------------------------------------------------------
void CMaterial::SetDiffuseTextureID(const u32 unTexID)
{
	m_unDiffuseTextureID = unTexID;
}
u32 CMaterial::GetDiffuseTextureID() const
{
	return m_unDiffuseTextureID;
}

void CMaterial::SetNormalTextureID(const u32 unTexID)
{
	m_unNormalTextureID = unTexID;
}
u32 CMaterial::GetNormalTextureID() const
{
	return m_unNormalTextureID;
}

//---------------------------------------------------------------------------
/*!
	@brief	透明度ありか
*/
//---------------------------------------------------------------------------
bool CMaterial::IsAlphaBlend() const
{
	//@todo ちゃんとできてないので要調整
	//return m_sMaterialData.flags
	return 1;
}

//EOF