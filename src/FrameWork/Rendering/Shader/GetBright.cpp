/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		GetBright.cpp
	@brief		輝度抽出シェーダ
	@author		shiba
	@date		2012/3/4 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "GetBright.h"
#include "Framework/DXUtil/UtilDX11.h"

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CGetBrightShader::CGetBrightShader()
	: mpNeverBuffer(nullptr)
	, mpEveryObjectBuffer(nullptr)
{
	m_eShaderID = CShaderManager::eSHADERID_GETBRIGHT;
}

//---------------------------------------------------------------------------
/*!
@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CGetBrightShader::~CGetBrightShader()
{
	SAFE_RELEASE(mpNeverBuffer);
	SAFE_RELEASE(mpEveryObjectBuffer);
}

//---------------------------------------------------------------------------
/*!
@brief	シェーダーロード
*/
//---------------------------------------------------------------------------
HRESULT CGetBrightShader::LoadShader()
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
	CreateShader(_T("Debug/GetBright.cso"), CShaderManager::eSHADERTYPE_PIXEL);

	//定数バッファの作成
	CUtilDX11::CreateConstantBuffer(sizeof(cbNeverChanges), &mpNeverBuffer);
	CUtilDX11::CreateConstantBuffer(sizeof(cbChangesEveryObject), &mpEveryObjectBuffer);

	return hr;
}

//---------------------------------------------------------------------------
/*!
@brief	シェーダオン
*/
//---------------------------------------------------------------------------
bool CGetBrightShader::ShaderOn()
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
void CGetBrightShader::SetProjection(NSMatrix& ProjMat)
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
void CGetBrightShader::SetView(NSMatrix& ViewMat)
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
void CGetBrightShader::SetLocal(NSMatrix& Local)
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

//EOF
