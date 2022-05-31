/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		MrtShader.cpp
	@brief		マルチレンダリングターゲット
	@author		shiba
	@date		2013/5/25 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "MrtShader.h"
#include "FrameWork/Rendering/ModelDataType.h"
#include "FrameWork/Rendering/Light.h"
#include "FrameWork/Rendering/RenderingManager.h"
//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CMrtShader::CMrtShader() :
	m_pCompareSmp(nullptr)
{
	m_eShaderID = CShaderManager::eSHADERID_MRT;
	for (int i = 0; i < E_BUFFER_PARAM::eMAX; ++i)
	{
		m_paBuffer[i] = nullptr;
	}
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CMrtShader::~CMrtShader()
{
	SAFE_RELEASE(m_pCompareSmp);
	for (int i = 0; i < E_BUFFER_PARAM::eMAX; ++i)
	{
		SAFE_RELEASE(m_paBuffer[i]);
	}
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダーロード
*/
//---------------------------------------------------------------------------
HRESULT CMrtShader::LoadShader()
{
	HRESULT hr = S_OK;

	LoadVertexShader();
	CreateShader(_T("Debug/Mrt_PS.cso"), CShaderManager::eSHADERTYPE_PIXEL);

	// サンプラーステートの生成.
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.BorderColor[0] = 1.0f;
		desc.BorderColor[1] = 1.0f;
		desc.BorderColor[2] = 1.0f;
		desc.BorderColor[3] = 1.0f;
		desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		desc.MaxAnisotropy = 1;
		desc.MipLODBias = 0;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = +FLT_MAX;

		// サンプラーステートを生成.
		DXMng* pcDXM = DXMng::GetInstance();
		hr = pcDXM->GetDevice().CreateSamplerState(&desc, &m_pCompareSmp);
		if (FAILED(hr))
		{
			NS_Report(_T("Error : ID3D11Device::CreateSamplerState() Failed."));
			return hr;
		}
	}

	return hr;
}
//---------------------------------------------------------------------------
/*!
	@brief	シェーダオン
*/
//---------------------------------------------------------------------------
bool CMrtShader::ShaderOn()
{
	//シェーダをパイプラインに設定
	if( !CShaderBase::ShaderOn() )
	{
		return false;
	}

	//シェーダ定数スロット設定
	ID3D11Buffer* pBuffer[E_BUFFER_PARAM::eMAX] = 
	{
		m_paBuffer[0],
		m_paBuffer[1],
		m_paBuffer[2],
	};

	DXMng* pDX = DXMng::GetInstance();
	pDX->GetConDevice().VSSetConstantBuffers( 0, E_BUFFER_PARAM::eMAX, pBuffer );
	pDX->GetConDevice().PSSetConstantBuffers( 0, E_BUFFER_PARAM::eMAX, pBuffer );

	// サンプラーステートを設定.
	pDX->GetConDevice().PSSetSamplers(1, 1, &m_pCompareSmp);

	//ライト取得、書き込み
	CRenderingManager* pcRenderM = CRenderingManager::GetInstance();
	if (pcRenderM != nullptr)
	{
		const CLight* pcLight = pcRenderM->GetDirectionalLight();
		if (pcLight != nullptr)
		{
			NSVector3 v = pcLight->GetPos();
			v.Normalize();
			SetLightPos(v);
		}
	}

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief		ワールド行列設定
	@param[in]	Local	ワールド行列
*/
//---------------------------------------------------------------------------
void CMrtShader::setWorld( const NSMatrix& World )
{
	m_cbMatrix.World = XMMatrixTranspose(World);;
	writeBuffer(eMATRIX);
}

//---------------------------------------------------------------------------
/*!
	@brief		ビュー行列設定
	@param[in]	ViewMat	ビュー行列
*/
//---------------------------------------------------------------------------
void CMrtShader::SetView( const NSMatrix& ViewMat )
{
	//コンパイル時、行列が列優先になっているので転置する
	m_cbMatrix.View = XMMatrixTranspose(ViewMat);
	writeBuffer(eMATRIX);
}

//---------------------------------------------------------------------------
/*!
	@brief	射影行列設定
	@param[in]	ProjMat	射影行列
*/
//---------------------------------------------------------------------------
void CMrtShader::SetProjection( const NSMatrix& ProjMat )
{
	//コンパイル時、行列が列優先になっているので転置する
	m_cbMatrix.Projection = XMMatrixTranspose(ProjMat);
	writeBuffer(eMATRIX);
}

//---------------------------------------------------------------------------
/*!
	@brief	ライト変換行列
	@param[in]	TexProj	ライトビュー行列
*/
//---------------------------------------------------------------------------
void CMrtShader::SetTexProj(const NSMatrix& TexProj)
{
	m_cbMatrix.TexProj = XMMatrixTranspose(TexProj);
	//m_cbMatrix.TexProj = TexProj;
	writeBuffer(eMATRIX);
}

//---------------------------------------------------------------------------
/*!
	@brief		スキンメッシュ用合成行列設定
	@param[in]	pCombMat	ワールド行列
*/
//---------------------------------------------------------------------------
void CMrtShader::SetCombMatrix(NSMatrix *pCombMat, const s32 nMatSize)
{
	if (pCombMat == nullptr)
	{
		return;
	}

	for (s32 i = 0; i < nMatSize; ++i)
	{
		m_cbMatrix.WorldSkin[i] = XMMatrixTranspose(pCombMat[i]);
	}
	writeBuffer(eMATRIX);
}

//---------------------------------------------------------------------------
/*!
	@brief	インスタンス描画用合成行列設定
	@param[in]	pMatrix	インスタンス用マトリクス

*/
//---------------------------------------------------------------------------
void CMrtShader::SetInstanceDrawMatrix(NSMatrix* pMatrix, const s32 nMatrixSize)
{
	//@todo 制限かけないと
	//	if( pcDX->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 )
	//	{
	//}

	if (INSTANCE_MODEL_MAX > nMatrixSize)
	{
		if (pMatrix)
		{
			D3D11_MAPPED_SUBRESOURCE pData;

			DXMng* pcDirect = DXMng::GetInstance();
			NS_ASSERT(pcDirect);

			pcDirect->GetConDevice().Map(m_paBuffer[eINSTANCE_MATRIX], 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);

			NSMatrix* m = (NSMatrix*)(pData.pData);
			for (s32 i = 0; i < nMatrixSize; i++)
			{
				m[i] = XMMatrixTranspose(pMatrix[i]);
			}
			pcDirect->GetConDevice().Unmap(m_paBuffer[eINSTANCE_MATRIX], 0);
		}
	}
	else
	{
		NS_Report(_T("Error! nMatrixSize is over INSTANCE_MAT"));
	}
}

//---------------------------------------------------------------------------
/*!
@brief	シェーダータイプ設定
*/
//---------------------------------------------------------------------------
void CMrtShader::SetVertShaderType(const s32 nVertShaderType)
{
	m_cParam.m_nShaderType = nVertShaderType;
	writeBuffer(ePARAM);
}

void CMrtShader::SetLightPos(const NSVector3& vPos)
{
	m_cParam.m_vLightPos = vPos;
	writeBuffer(ePARAM);
}

void CMrtShader::LoadVertexShader()
{
	//入力レイアウト設定
	//頂点カラー、法線、テクスチャUV、影響率、影響ボーンインデックス、シェーダインスタンスマトリクス
	u32 nColorOffset, nNormalOffset, nTexOffset, nBoneWeight, nBoneIndex;
	nColorOffset = nNormalOffset = nTexOffset = nBoneIndex = nBoneWeight = 0;

	nTexOffset = sizeof(NSVector3);
	nNormalOffset = sizeof(NSVector3) + sizeof(NSVector2);
	nBoneWeight = sizeof(NSVector3) + sizeof(NSVector2) + sizeof(NSVector3);
	nBoneIndex = sizeof(NSVector3) + sizeof(NSVector2) + sizeof(NSVector3) + sizeof(f32) * 3;

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, nTexOffset, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, nNormalOffset, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, nColorOffset, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, nBoneWeight, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, nBoneIndex, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		//インスタンシング用設定
		{ "MATRIX",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,  0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX",   1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX",   2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX",   3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};

	UINT LayOutSize = _countof(layout);

	//頂点シェーダ生成の際は、
	//IAに入力されるデータと頂点シェーダの入力の整合性が取れているか、入力レイアウトを生成する

	CreateShader(_T("Debug/Mrt_VS.cso"), CShaderManager::eSHADERTYPE_VERTEX, layout, LayOutSize);
	CreateShader(_T("Debug/Mrt_PS.cso"), CShaderManager::eSHADERTYPE_PIXEL);

	//定数バッファの作成
	CUtilDX11::CreateConstantBuffer(sizeof(cbMatrix), &m_paBuffer[eMATRIX]);
	CUtilDX11::CreateConstantBuffer(sizeof(cbParam), &m_paBuffer[ePARAM]);

	//@note インスタンス分マトリクスを生成して使い回す。
	CUtilDX11::CreateConstantBuffer(sizeof(NSMatrix) * INSTANCE_MODEL_MAX, &m_paBuffer[eINSTANCE_MATRIX]);

	for (s32 i = 0; i < BONE_MAX; ++i)
	{
		m_cbMatrix.WorldSkin[i] = DirectX::XMMatrixIdentity();	//一旦初期化
	}
}

//---------------------------------------------------------------------------
/*!
@brief	バッファに書き込み
*/
//---------------------------------------------------------------------------
void CMrtShader::writeBuffer(const E_BUFFER_PARAM eBufferParamID)
{
	D3D11_MAPPED_SUBRESOURCE MappedResource;

	if (FAILED(CUtilDX11::Map(m_paBuffer[eBufferParamID], D3D11_MAP_WRITE_DISCARD, &MappedResource)))
	{
		NS_ASSERT(0);
	}

	if (eBufferParamID == eMATRIX)
	{
		CopyMemory(MappedResource.pData, &m_cbMatrix, sizeof(cbMatrix));
	}
	else if (eBufferParamID == ePARAM)
	{
		CopyMemory(MappedResource.pData, &m_cParam, sizeof(cbParam));
	}

	CUtilDX11::UnMap(m_paBuffer[eBufferParamID]);
}

//EOF
