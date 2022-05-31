/*==========================================================================*/
/*==========================================================================*/
/*!
@file		Material.cpp
@brief		マテリアルクラス
@author		shiba
@date		2012/9/22 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "Material.h"

#include "FrameWork/Rendering/Shader/ShaderBase.h"
#include "FrameWork/Asset/AssetManager.h"
#include "FrameWork/Asset/AssetTexture.h"

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CMaterial::CMaterial()
	: m_sMaterialData()
	, m_pcShader(nullptr)
	, m_unDiffuseTextureID(INVALID_U32)
	, m_unNormalTextureID(INVALID_U32)
#if defined( _DEBUG )
	, m_pcDebugView(nullptr)
#endif
{
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CMaterial::~CMaterial()
{
	m_pcShader = nullptr;
#if defined( _DEBUG )
	m_pcDebugView = nullptr;
#endif
}

//---------------------------------------------------------------------------
/*!
	@brief	初期化
*/
//---------------------------------------------------------------------------
bool CMaterial::Init()
{
	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	描画パイプラインに適用
*/
//---------------------------------------------------------------------------
void CMaterial::OnMaterial()
{
#if defined( _DEBUG )
	DXMng* pcDXM = DXMng::GetInstance();
	NS_ASSERT(pcDXM);

	CAssetManager* pcResM = CAssetManager::GetInstance();
	NS_ASSERT(pcResM);

	if (m_pcDebugView)
	{
		//デバッグ用ビュー
		pcDXM->GetConDevice().PSSetShaderResources(0, 1, &m_pcDebugView);
	}
#endif

	if (m_pcShader)
	{
		m_pcShader->ShaderOn();
	}
}

//---------------------------------------------------------------------------
/*!
	@brief	描画パイプラインからはずす
*/
//---------------------------------------------------------------------------
void CMaterial::OffMaterial()
{
	if (m_pcShader)
	{
		m_pcShader->ShaderOff();
	}

	DXMng* pcDXM = DXMng::GetInstance();
	NS_ASSERT(pcDXM);

	//@note ステージ終了後に移動
	//@note こいつがDefferdのためにセットしたのをリセットしてた
	//@note 色々テクスチャを設定したあと消されるのつらい
	//for (s32 i = 0; i < TEXTURE_MAX_NUM; i++)
	//{
	//	//nullptrをセットしておく
	//	ID3D11ShaderResourceView* pcResoure = nullptr;
	//	pcDXM->GetConDevice().PSSetShaderResources(i, 1, &pcResoure);
	//}
}

//EOF
