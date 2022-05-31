/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		CAssetMesh.cpp
	@brief		メッシュ管理
	@author		shiba
	@date		2013/4/28 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "AssetMesh.h"

#include "FrameWork/DXUtil/DXMng.h"
#include "FrameWork/Rendering/Material.h"

#include "FrameWork/Rendering/Model.h"
#include "FrameWork/Rendering/Shader/ShaderBase.h"
#include "FrameWork/Rendering/Shader/ShaderBinder.h"
#include "FrameWork/Rendering/Shader/ShaderManager.h"

#include "FrameWork/Rendering/Shader/MrtShader.h"
#include "FrameWork/Rendering/Shader/ZRenderShader.h"

#include "FrameWork/Asset/AssetManager.h"
#include "FrameWork/Asset/AssetTexture.h"

using namespace DirectX;

CAssetMesh::CAssetMesh() :
	m_ePrimType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_unapIndecies(nullptr),
	m_unIndexCount(0),
	m_pVtxBuffer(nullptr),
	m_pIdxBuffer(nullptr),
	m_unVtxStride(sizeof(BasicVertex)),
	m_naMaterialIndex(nullptr),
	m_nMaterialIndexCount(0),
	m_bSkinMesh(false)
{
	m_naMaterialIndex = new s32[TEXTURE_MAX_NUM];
	for (s32 i = 0; i < TEXTURE_MAX_NUM; ++i)
	{
		m_naMaterialIndex[i] = -1;
	}
}

CAssetMesh::~CAssetMesh()
{
	Term();
}

//---------------------------------------------------------------------------
/*!
@brief	破棄
*/
//---------------------------------------------------------------------------
void CAssetMesh::Term()
{
	SAFE_DELETEA(m_naMaterialIndex);

	SAFE_RELEASE(m_pVtxBuffer);
	SAFE_RELEASE(m_pIdxBuffer);

	SAFE_DELETEA(m_unapIndecies);

	m_caVertices.clear();
}

//---------------------------------------------------------------------------
/*!
@brief	描画バッファ作成
*/
//---------------------------------------------------------------------------
bool CAssetMesh::CreateBuffer()
{
	//--------------------------------------------------------------------
	// 頂点バッファ作成
	//--------------------------------------------------------------------
	if (m_unVtxStride > 0 && m_caVertices.size() > 0)
	{
		SAFE_RELEASE(m_pVtxBuffer);
		CUtilDX11::CreateVertexBuffer(
			&m_pVtxBuffer,
			m_unVtxStride * m_caVertices.size(),
			&m_caVertices[0]); //@note m_pVertices[0]と&m_pVerticesは違うのだ。
	}
	else
	{
		return false;
	}

	//--------------------------------------------------------------------
	// インデックスバッファ作成
	//--------------------------------------------------------------------
	if (m_unIndexCount > 0 && m_unapIndecies)
	{
		SAFE_RELEASE(m_pIdxBuffer);
		CUtilDX11::CreateIndexBuffer(
			&m_pIdxBuffer,
			sizeof(u16) * m_unIndexCount,
			(void*)m_unapIndecies);
	}
	else
	{
		return false;
	}

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	メッシュのコピー
*/
//---------------------------------------------------------------------------
bool CAssetMesh::CopyMesh(CAssetMesh* pcCopyDestMesh)
{
	bool bSuccess = false;
	if (pcCopyDestMesh)
	{
		m_ePrimType = pcCopyDestMesh->m_ePrimType;

		m_caVertices.clear();
		std::vector< BasicVertex >::iterator it = pcCopyDestMesh->m_caVertices.begin();
		for (; it != pcCopyDestMesh->m_caVertices.end(); ++it)
		{
			m_caVertices.push_back((*it));
		}

		SAFE_DELETEA(m_unapIndecies);
		m_unapIndecies = new u16[pcCopyDestMesh->m_unIndexCount];
		for (u32 i = 0; i < pcCopyDestMesh->m_unIndexCount; ++i)
		{
			m_unapIndecies[i] = pcCopyDestMesh->m_unapIndecies[i];
		}

		m_unIndexCount = pcCopyDestMesh->m_unIndexCount;
		m_unVtxStride = pcCopyDestMesh->m_unVtxStride;

		bSuccess = true;
	}

	return bSuccess;
}

//---------------------------------------------------------------------------
/*!
	@brief		頂点データの設定
	@param[in]	caVertexs	頂点データ
*/
//---------------------------------------------------------------------------
bool CAssetMesh::SetVertexData(BasicVertex caVertexs[], const s32 nArraySize)
{
	bool bSuccess = false;
	
	if (caVertexs != nullptr)
	{
		if (nArraySize > 0)
		{
			m_caVertices.clear();

			for (s32 i = 0; i < nArraySize; ++i)
			{
				m_caVertices.push_back(caVertexs[i]);
			}

			bSuccess = true;
		}
	}

	return bSuccess;
}

//---------------------------------------------------------------------------
/*!
	@brief		インデックスデータの設定
	@param[in]	caIndecies	頂点データ
*/
//---------------------------------------------------------------------------
bool CAssetMesh::SetIndexData(u16 caIndecies[], const s32 nArraySize)
{
	bool bSuccess = false;
	if (caIndecies != nullptr)
	{
		if (nArraySize > 0)
		{
			m_unIndexCount = nArraySize;
			SAFE_DELETEA(m_unapIndecies);

			m_unapIndecies = new u16[nArraySize];
			for (s32 i = 0; i < nArraySize; ++i)
			{
				m_unapIndecies[i] = caIndecies[i];
			}
			bSuccess = true;
		}
	}

	return bSuccess;
}

//---------------------------------------------------------------------------
/*!
	@brief		描画
	@note		メッシュの描画処理。単体の描画処理はここに統一
	@param[in]	pcRender		描画インターフェイス
	@param[in]	bInstanceDraw	インスタンス描画
	@param[in]	pcMat			このAssetMeshが使用しうるマテリアル一覧(今0バンのみアクセス
*/
//---------------------------------------------------------------------------
bool CAssetMesh::Draw(CRender* pcRender, const bool bInstanceDraw, class CMaterial* pcMat[])
{
	bool bSuccess = false;
	if(m_pIdxBuffer && m_pIdxBuffer && pcMat)
	{
		DXMng* pcDXM = DXMng::GetInstance();
		NS_ASSERT(pcDXM);

		//@todo 1メッシュ2シェーダ以上には対応していないので注意。
		//メッシュが所持するマテリアルを適用
		pcMat[0]->OnMaterial();

		CAssetManager* pcResM = CAssetManager::GetInstance();

#if false
		if (m_unapIndecies != nullptr && pcResM != nullptr)
		{
			//テクスチャ設定
			for (s32 i = 0; i < TEXTURE_MAX_NUM; i++)
			{
				if (m_naMaterialIndex[i] >= 0)
				{
					CMaterial* pcUseMat = pcMat[m_naMaterialIndex[i]];
					if (pcUseMat != nullptr)
					{
						CAssetTexture* pcTex = static_cast<CAssetTexture*>(pcResM->GetAsset(pcUseMat->GetDiffuseTextureID()));
						if (pcTex)
						{
							//@todo ここを複数対応にする必要あり？
							ID3D11ShaderResourceView* pcResource = pcTex->GetTextureRevi();
							pcDXM->GetConDevice().PSSetShaderResources(i, 1, &pcResource);
						}
					}
				}
			}
		}
#else
		if (m_unapIndecies != nullptr && pcResM != nullptr)
		{
			//@todo 必要なら1メッシュマルチマテリアルに対応する必要あり
			//ノーマルとかスペキュラマップ等使うならマルチマテリアル
			if (m_naMaterialIndex[0] >= 0)
			{
				CMaterial* pcUseMat = pcMat[0];
				if (pcUseMat != nullptr)
				{
					CAssetBase* pcAsset = pcResM->GetAsset(pcUseMat->GetDiffuseTextureID());
					if (pcAsset != nullptr)
					{
						CAssetTexture* pcAssetTex = static_cast<CAssetTexture*>(pcAsset);
						if (pcAssetTex)
						{
							ID3D11ShaderResourceView* pcResource = pcAssetTex->GetTextureRevi();
							pcDXM->GetConDevice().PSSetShaderResources(0, 1, &pcResource);
						}
					}
				}
			}
		}
#endif
		pcDXM->GetConDevice().IASetPrimitiveTopology(m_ePrimType);

		CShaderBase* pcShader = pcMat[0]->GetShader();
		if (pcShader)
		{
			//シェーダにパラメーターを適用する
			CShaderBinder::BindShaderParam(pcRender, pcShader, this);

			if (bInstanceDraw)
			{
				//インスタンシング用処理
				//@todo 汎用性ないので治したい
				CModel * pcModel = NS_STATIC_CAST(CModel*, pcRender);
				CShaderManager* pcShaderM = CShaderManager::GetInstance();
				NS_ASSERT(pcShaderM);

				//@todo 修正
				ID3D11Buffer* pInstanceBuffer = nullptr;
				if (pcShader->GetShaderID() == CShaderManager::eSHADERID_MRT)
				{
					CMrtShader* pcInstanceMatShdaer= NS_STATIC_CAST(CMrtShader*, pcShader);
					NS_ASSERT(pcInstanceMatShdaer);
					pInstanceBuffer = pcInstanceMatShdaer->GetInstanceMatBuffer();
				}
				else if (pcShader->GetShaderID() == CShaderManager::eSHADERID_ZRENDER)
				{
					CZRenderShader* pcInstanceMatShdaer = NS_STATIC_CAST(CZRenderShader*, pcShader);
					NS_ASSERT(pcInstanceMatShdaer);
					pInstanceBuffer = pcInstanceMatShdaer->GetInstanceMatBuffer();
				}

				//バッファ配列
				ID3D11Buffer* pBufferTbl[] = { m_pVtxBuffer, pInstanceBuffer };
				//バッファサイズ
				UINT SizeTbl[] = { m_unVtxStride, sizeof(NSMatrix) };
				//どこから使用するか定義
				UINT OffsetTbl[] = { 0, 0 };

				pcDXM->GetConDevice().IASetVertexBuffers(0, 2, pBufferTbl, SizeTbl, OffsetTbl);
				pcDXM->GetConDevice().IASetIndexBuffer(m_pIdxBuffer, DXGI_FORMAT_R16_UINT, 0);
				//インスタンシング描画
				pcDXM->GetConDevice().DrawIndexedInstanced(m_unIndexCount, pcModel->GetInstancingNum(), 0, 0, 0);
			}
			else
			{
				// 通常描画処理
				// 頂点バッファを設定(入力レイアウトの設定
				ID3D11Buffer* pBufferTbl[] = { m_pVtxBuffer };		//バッファ配列
				UINT SizeTbl[] = { m_unVtxStride };					//バッファサイズ
				UINT OffsetTbl[] = { 0 };											//どこから使用するか定義
				pcDXM->GetConDevice().IASetVertexBuffers(0, 1, pBufferTbl, SizeTbl, OffsetTbl);

				// 入力アセンブラステージにインデックスバッファーをバインド
				pcDXM->GetConDevice().IASetIndexBuffer(m_pIdxBuffer, DXGI_FORMAT_R16_UINT, 0);

				// 描画コマンド発行
				pcDXM->GetConDevice().DrawIndexed(m_unIndexCount, 0, 0);
			}

			bSuccess = true;
		}

		//pcDXM->GetConDevice().Flush();		//呼ばなくていい
		//pcDXM->GetConDevice().ClearState();	//破棄とかに呼び出す

		pcMat[0]->OffMaterial();
	}

	return bSuccess;
}

//---------------------------------------------------------------------------
/*!
	@brief		マテリアルインデックスデータの設定
	@param[in]	nMaterialIndex	マテリアルのインデックス
*/
//---------------------------------------------------------------------------
void CAssetMesh::SetMaterialIndex(const s32 nMaterialIndex)
{
	if (m_nMaterialIndexCount >= 0 && m_nMaterialIndexCount < TEXTURE_MAX_NUM)
	{
		m_naMaterialIndex[m_nMaterialIndexCount] = nMaterialIndex;
		m_nMaterialIndexCount++;
	}
}

//EOF
