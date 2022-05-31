/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		Sprite.cpp
	@brief		スプライトクラス
	@author		shiba
	@date		2012/3/3 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Rendering/Sprite.h"
#include "FrameWork/Rendering/Material.h"


#include "FrameWork/Asset/AssetManager.h"
#include "FrameWork/Asset/AssetMesh.h"
#include "FrameWork/Asset/AssetTexture.h"

u32 CSprite::m_nSpriteCount;

CSprite::CSprite()
: m_pcMesh(nullptr)
, m_pcMat(nullptr)
{}

CSprite::~CSprite()
{
	Term();
}

//---------------------------------------------------------------------------
/*!
@brief	破棄
*/
//---------------------------------------------------------------------------
void CSprite::Destroy()
{
	Term();
}

void CSprite::Term()
{
	SAFE_DELETE(m_pcMat);

	CAssetManager* pcAssetM = CAssetManager::GetInstance();
	if (pcAssetM != nullptr)
	{
		if (!pcAssetM->RemoveAsset(m_pcMesh))
		{
#if defined(_DEBUG)
			NS_Report(_T("Remove Error! Sprite\n"));
#endif
		}
	}
}

//---------------------------------------------------------------------------
/*!
	@brief	初期化
*/
//---------------------------------------------------------------------------
bool CSprite::Init()
{
	bool bSuccess = CRender::Init();
	if( !bSuccess)
	{
		return bSuccess;
	}

	//マテリアル生成
	CLASS_NEW(m_pcMat, CMaterial());

	//四角形のポリゴンを生成
	TCHAR strFileName[64] = { 0 };
	wsprintf(strFileName, _T("SpriteMesh[%d]\n"), m_nSpriteCount++);

	CLASS_NEW(m_pcMesh, CAssetMesh);
	m_pcMesh->SetAssetName(strFileName);
	m_pcMesh->SetAssetUID(CUtilHash::calcNameHash(strFileName));
	m_pcMesh->SetMaterialIndex(0);

	CAssetManager* pcAssetM = CAssetManager::GetInstance();
	if (pcAssetM != nullptr)
	{
		pcAssetM->EntryAsset(m_pcMesh);
	}

	//頂点データ指定
	//@todo セーフ配列使ったほうがよさげ
	BasicVertex VertexDataA[] =
	{
		//Pos,UV,Normal
		BasicVertex(NSVector3(-0.5f, -0.5f, 0.0f),	NSVector2(0.0f, 0.0f), NSVector3(0.0f, 0.0f, 1.0f)),
		BasicVertex(NSVector3(0.5f, -0.5f, 0.0f),	NSVector2(1.0f, 0.0f), NSVector3(0.0f, 0.0f, 1.0f)),
		BasicVertex(NSVector3(-0.5f, 0.5f,0.0f),	NSVector2(0.0f, 1.0f), NSVector3(0.0f, 0.0f, 1.0f)),
		BasicVertex(NSVector3(0.5f, 0.5f,	0.0f),	NSVector2(1.0f, 1.0f), NSVector3(0.0f, 0.0f, 1.0f)),
	};
	bSuccess = m_pcMesh->SetVertexData(VertexDataA, SIZE_OF_ARRAY(VertexDataA));
	if (!bSuccess)
	{
		return bSuccess;
	}

	//インデックスデータ指定、生成
	//u16 caIndecies[6] =
	//{
	//	0,1,2,1,2,3
	//};
	//u16 caIndecies[6] =
	//{
	//	0,1,2,1,3,2
	//};
	u16 caIndecies[6] =
	{
		2,1,0,2,3,1
	};
	bSuccess = m_pcMesh->SetIndexData(caIndecies, SIZE_OF_ARRAY(caIndecies));
	if (!bSuccess)
	{
		return bSuccess;
	}

	//メッシュ生成
	bSuccess = m_pcMesh->CreateBuffer();

	return bSuccess;
}

//---------------------------------------------------------------------------
/*!
	@brief	更新
*/
//---------------------------------------------------------------------------
void CSprite::Update( const f32 fDelta )
{
	CRender::Update(fDelta);
}

//---------------------------------------------------------------------------
/*!
	@brief	描画
*/
//---------------------------------------------------------------------------
void CSprite::Draw()
{	
	if( m_pcMesh )
	{
		//保存しておいて戻す
		m_cRenderContext.ApplyRenderDesc();

		//マテリアル指定
		if (m_pcMat)
		{
			//描画設定の取得
			const CRenderingContext& conText = GetRenderContext();
			m_pcMat->SetShader(conText.GetShader());
		}

		//描画登録
		m_pcMesh->Draw( this, false, &m_pcMat );

		CUtilDX11::SetAllDefaultState();
	}
}

//---------------------------------------------------------------------------
/*!
	@brief	テクスチャデータ指定
	@param[in]	pcTexture	テクスチャ
*/
//---------------------------------------------------------------------------
bool CSprite::SetTexture(CAssetTexture* pcTexture)
{
	if (m_pcMat && pcTexture)
	{
		m_pcMat->SetDiffuseTextureID(pcTexture->GetAssetUID());
	}

	return true;
}

#if defined( _DEBUG )
//---------------------------------------------------------------------------
/*!
@brief	テクスチャデータ指定
@param[in]	pcResource	テクスチャリソース
*/
//---------------------------------------------------------------------------
bool CSprite::SetTexture(ID3D11ShaderResourceView* pcResource)
{
	if (m_pcMat)
	{
		m_pcMat->SetDebugView(pcResource);
	}

	return true;
}
#endif
//---------------------------------------------------------------------------
/*!
	@brief	UV指定
*/
//---------------------------------------------------------------------------
bool CSprite::SetUV( const f32 u, const f32 v, const f32 u2, const f32 v2 )
{
	BasicVertex VertexDataA[] = 
	{
		//UVを指定して頂点データ作り直す
		BasicVertex(NSVector3(-0.5f, -0.5f, 0.0f),	NSVector2(u, v), NSVector3(0.0f, 0.0f, 1.0f)),
		BasicVertex(NSVector3(0.5f, -0.5f, 0.0f),	NSVector2(u2, v), NSVector3(0.0f, 0.0f, 1.0f)),
		BasicVertex(NSVector3(-0.5f, 0.5f,0.0f),	NSVector2(u, v2), NSVector3(0.0f, 0.0f, 1.0f)),
		BasicVertex(NSVector3(0.5f, 0.5f,	0.0f),	NSVector2(u2, v2), NSVector3(0.0f, 0.0f, 1.0f)),
	};

	bool bSuccess = m_pcMesh->SetVertexData(VertexDataA, SIZE_OF_ARRAY(VertexDataA));
	if (bSuccess)
	{
		bSuccess = m_pcMesh->CreateBuffer();
	}
	
	return bSuccess;
}

//EOF
 