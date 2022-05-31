/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		BlurBright.cpp
	@brief		ブラー
	@author		shiba
	@date		2012/3/4 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "BlurShader.h"
#include "Framework/DXUtil/UtilDX11.h"
#include <math.h>

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CBlurShader::CBlurShader()
	: mpNeverBuffer(nullptr)
	, mpEveryObjectBuffer(nullptr)
	, mpGaussParamBuffer(nullptr)
	, mpGaussDrawType(nullptr)
{
	m_eShaderID = CShaderManager::eSHADERID_BLUR;
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CBlurShader::~CBlurShader()
{
	SAFE_RELEASE(mpGaussDrawType);
	SAFE_RELEASE(mpGaussParamBuffer);
	SAFE_RELEASE(mpNeverBuffer);
	SAFE_RELEASE(mpEveryObjectBuffer);
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダーロード
*/
//---------------------------------------------------------------------------
HRESULT CBlurShader::LoadShader()
{
	HRESULT hr = S_OK;

	//入力レイアウトの設定
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,						D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD"	, 0, DXGI_FORMAT_R32G32_FLOAT,		0, sizeof(NSVector3),		D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT LayOutSize = _countof(layout);

	//シェーダファイルのコンパイル
	CreateShader(_T("Debug/SpriteShader_VS.cso"), CShaderManager::eSHADERTYPE_VERTEX, layout, LayOutSize);
	CreateShader(_T("Debug/Blur.cso"), CShaderManager::eSHADERTYPE_PIXEL);

	//定数バッファの作成
	CUtilDX11::CreateConstantBuffer(sizeof(cbNeverChanges), &mpNeverBuffer);
	CUtilDX11::CreateConstantBuffer(sizeof(cbChangesEveryObject), &mpEveryObjectBuffer);
	CUtilDX11::CreateConstantBuffer(sizeof(cbGaussParamObject), &mpGaussParamBuffer);
	CUtilDX11::CreateConstantBuffer(sizeof(cbGaussDrawType), &mpGaussDrawType);

	//ガウスパラメータの作成
	//遠くにいくほど薄くなるように。重み
	f32 fTotalWeight = 0.0f;

#if true
	f32 d = 15000.0f / 100.0f;	//これが大きければおおきくなるほど強いぼかし
	for (s32 i = 0; i < SAMPLE_COUNT; i++)
	{
		f32 r = 1.0f + 2.0f * i;
		f32 w = exp(-0.5f * (r * r) / d);
#if defined(GAUSSPARAM_ALIGN)
		m_cGaussParam.offsetWeights[i] = DirectX::XMVectorSet(w, 0, 0, 0);
#else
		m_cGaussParam.weights[i] = w;
#endif
		if (i > 0) { w *= 2.0f; }
		fTotalWeight += w;
	}
#else
	for (s32 i = 0; i < SAMPLE_COUNT; i++)
	{
		f32 p = (i - (SAMPLE_COUNT - 1) * 0.5f) * 0.0006f;

#if defined(GAUSSPARAM_ALIGN)
		m_cGaussParam.offsetWeights[i] = 
			DirectX::XMVectorSet(exp(-p * p / 2) / sqrt(UtilMath::PI * 2), 0,0,0);
		fTotalWeight += XMVectorGetX(m_cGaussParam.offsetWeights[i]);
#else
		m_cGaussParam.weights[i] = exp(-p * p / 2) / sqrt(UtilMath::PI * 2);
		fTotalWeight += m_cGaussParam.weights[i];
#endif
	}
#endif
	for (s32 i = 0; i < SAMPLE_COUNT; ++i)
	{
#if defined(GAUSSPARAM_ALIGN)
		m_cGaussParam.offsetWeights[i] = DirectX::XMVectorSet(
			XMVectorGetX(m_cGaussParam.offsetWeights[i]) / fTotalWeight, 0, 0, 0);
#else
		m_cGaussParam.weights[i] /= fTotalWeight;
#endif
	}

	f32 fSamplingLength = 0.002f;	//サンプリングオフセットの長さ
	for (s32 i = 0; i < SAMPLE_COUNT; ++i)
	{

#if defined(GAUSSPARAM_ALIGN)
		m_cGaussParam.offsetWeights[i] = DirectX::XMVectorSet(
			XMVectorGetX(m_cGaussParam.offsetWeights[i]), 
			i * fSamplingLength - fSamplingLength*SAMPLE_COUNT*0.5f, 
			i * fSamplingLength - fSamplingLength*SAMPLE_COUNT*0.5f, 0);
#else
		m_cGaussParam.offset[i].x = i * fSamplingLength - fSamplingLength*SAMPLE_COUNT*0.5f;
		m_cGaussParam.offset[i].y = i * fSamplingLength - fSamplingLength*SAMPLE_COUNT*0.5f;
#endif
	}
	WriteGaussParam();

	return hr;
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダオン
*/
//---------------------------------------------------------------------------
bool CBlurShader::ShaderOn()
{
	//シェーダをパイプラインに設定
	if (!CShaderBase::ShaderOn())
	{
		return false;
	}

	//シェーダ定数スロット設定
	ID3D11Buffer* pBuffer[CONBUFFERNUM] =
	{
		mpNeverBuffer,
		mpEveryObjectBuffer,
		mpGaussParamBuffer,
		mpGaussDrawType
	};

	DXMng* pDX = DXMng::GetInstance();
	pDX->GetConDevice().VSSetConstantBuffers(0, CONBUFFERNUM, pBuffer);
	pDX->GetConDevice().PSSetConstantBuffers(0, CONBUFFERNUM, pBuffer);

	return true;
}


//---------------------------------------------------------------------------
/*!
	@brief	射影行列設定
	@param[in]	ProjMat	射影行列
*/
//---------------------------------------------------------------------------
void CBlurShader::SetProjection(NSMatrix& ProjMat)
{
	m_cNeverChange.Projection = XMMatrixTranspose(ProjMat);

	ID3D11Buffer* pBuffer = mpNeverBuffer;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HRESULT hr = CUtilDX11::Map(pBuffer, D3D11_MAP_WRITE_DISCARD, &MappedResource);
	if (FAILED(hr))
	{
		return;
	}

	CopyMemory(MappedResource.pData, &m_cNeverChange, sizeof(cbNeverChanges));

	CUtilDX11::UnMap(pBuffer);
}

//---------------------------------------------------------------------------
/*!
	@brief		ビュー行列設定
	@param[in]	ViewMat	ビュー行列
*/
//---------------------------------------------------------------------------
void CBlurShader::SetView(NSMatrix& ViewMat)
{
	m_cNeverChange.View = XMMatrixTranspose(ViewMat);

	ID3D11Buffer* pBuffer = mpNeverBuffer;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HRESULT hr = CUtilDX11::Map(pBuffer, D3D11_MAP_WRITE_DISCARD, &MappedResource);
	if (FAILED(hr))
	{
		return;
	}

	CopyMemory(MappedResource.pData, &m_cNeverChange, sizeof(cbNeverChanges));

	CUtilDX11::UnMap(pBuffer);
}

//---------------------------------------------------------------------------
/*!
	@brief		ローカル行列設定
	@param[in]	Local	ローカル行列
*/
//---------------------------------------------------------------------------
void CBlurShader::SetLocal(NSMatrix& Local)
{
	m_cEveryChange.World = XMMatrixTranspose(Local);

	ID3D11Buffer* pBuffer = mpEveryObjectBuffer;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HRESULT hr = CUtilDX11::Map(pBuffer, D3D11_MAP_WRITE_DISCARD, &MappedResource);
	if (FAILED(hr))
	{
		return;
	}

	CopyMemory(MappedResource.pData, &m_cEveryChange, sizeof(cbChangesEveryObject));

	CUtilDX11::UnMap(pBuffer);

}

void CBlurShader::SetBlurType(const s32 nBlurType)
{
	m_cGaussDrawType.nBlurType = nBlurType;
	WriteGaussDrawType();
}

void CBlurShader::WriteGaussParam()
{
	ID3D11Buffer* pBuffer = mpGaussParamBuffer;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HRESULT hr = CUtilDX11::Map(pBuffer, D3D11_MAP_WRITE_DISCARD, &MappedResource);
	if (FAILED(hr))
	{
		return;
	}

	CopyMemory(MappedResource.pData, &m_cGaussParam, sizeof(cbGaussParamObject));

	CUtilDX11::UnMap(pBuffer);
}

void CBlurShader::WriteGaussDrawType()
{
	ID3D11Buffer* pBuffer = mpGaussDrawType;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HRESULT hr = CUtilDX11::Map(pBuffer, D3D11_MAP_WRITE_DISCARD, &MappedResource);
	if (FAILED(hr))
	{
		return;
	}

	CopyMemory(MappedResource.pData, &m_cGaussDrawType, sizeof(cbGaussDrawType));

	CUtilDX11::UnMap(pBuffer);
}

//EOF
