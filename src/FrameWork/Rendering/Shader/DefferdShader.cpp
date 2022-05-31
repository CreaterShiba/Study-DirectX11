/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		DefferdShader.cpp
	@brief		DefferdShading
	@author		shiba
	@date		2013/06/18 create this file.
*/
/*==========================================================================*/
/*==========================================================================*/
#include "DefferdShader.h"
#include "FrameWork/DXUtil/DXMng.h"

using namespace DirectX;

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CDefferdShader::CDefferdShader() 
: mpLightBuffer( nullptr )
{
	m_eShaderID = CShaderManager::eSHADERID_DEFFERD;
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CDefferdShader::~CDefferdShader()
{
	SAFE_RELEASE( mpLightBuffer );
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダーロード
*/
//---------------------------------------------------------------------------
HRESULT CDefferdShader::LoadShader()
{
	HRESULT hr = S_OK;

	//入力レイアウトの設定
	D3D11_INPUT_ELEMENT_DESC layout[] = 
	{
		{"POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,					D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD"	, 0, DXGI_FORMAT_R32G32_FLOAT,		0, sizeof(NSVector3),	D3D11_INPUT_PER_VERTEX_DATA, 0},
	}; 

	UINT LayOutSize = _countof( layout );

	//頂点シェーダ生成の際は、
	//IAに入力されるデータと頂点シェーダの入力の整合性が取れているか、入力レイアウトを生成する
	CreateShader(_T("Debug/DefferedShader_VS.cso"), CShaderManager::eSHADERTYPE_VERTEX, layout, LayOutSize);
	CreateShader(_T("Debug/DefferedShader_PS.cso"), CShaderManager::eSHADERTYPE_PIXEL);


	//定数バッファの作成
	CUtilDX11::CreateConstantBuffer(sizeof(cbLightObject), &mpLightBuffer);

	for (s32 i = 0; i < CRenderingManager::eLIGHT_MAX; ++i)
	{
		m_LightObj.pos[i] = XMVectorSet(-1000.0f, -1000.0f, -1000.0f, -1000.0f);
	}

	return hr;
}
//---------------------------------------------------------------------------
/*!
	@brief	シェーダオン
*/
//---------------------------------------------------------------------------
bool CDefferdShader::ShaderOn()
{
	//シェーダをパイプラインに設定
	if( !CShaderBase::ShaderOn() )
	{
		return false;
	}

	//シェーダ定数スロット設定
	ID3D11Buffer* pBuffer[CONBUFFERNUM] = 
	{
		mpLightBuffer,
	};

	DXMng* pDX = DXMng::GetInstance();
	pDX->GetConDevice().VSSetConstantBuffers( 0, CONBUFFERNUM, pBuffer );
	pDX->GetConDevice().PSSetConstantBuffers( 0, CONBUFFERNUM, pBuffer );
	
	return true;
}

void CDefferdShader::setLightPos( const s32 idx, const NSVector3& lightPos )
{
	if (idx >= 0 && idx < CRenderingManager::eLIGHT_MAX)
	{
		m_LightObj.pos[idx] = XMVectorSet(lightPos.x, lightPos.y, lightPos.z, 1.0f);
		//m_LightObj.pos = XMVectorSet(lightPos.x, lightPos.y, lightPos.z, 1.0f);
		WriteLightInfo();
	}
}

void CDefferdShader::setLightCol( const s32 idx, const SColor& lightCol )
{
	if (idx >= 0 && idx < CRenderingManager::eLIGHT_MAX)
	{
		m_LightObj.col[idx] = XMVectorSet(
			XMVectorGetX(lightCol),
			XMVectorGetY(lightCol),
			XMVectorGetZ(lightCol),
			XMVectorGetW(m_LightObj.col[idx]));
		WriteLightInfo();
	}
}

void CDefferdShader::SetLightPow(const s32 idx, const f32 fPow)
{
	if (idx >= 0 && idx < CRenderingManager::eLIGHT_MAX)
	{
		m_LightObj.col[idx] = XMVectorSet(
			XMVectorGetX(m_LightObj.col[idx]),
			XMVectorGetY(m_LightObj.col[idx]),
			XMVectorGetZ(m_LightObj.col[idx]),
			fPow);
		WriteLightInfo();
	}
}

void CDefferdShader::setCamPos( const NSVector4& camPos )
{
	m_LightObj.campos = camPos;
	WriteLightInfo();
}

void CDefferdShader::WriteLightInfo()
{
	ID3D11Buffer* pBuffer = mpLightBuffer;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	if (FAILED(CUtilDX11::Map(pBuffer, D3D11_MAP_WRITE_DISCARD, &MappedResource)))
	{
		NS_ASSERT(0);
	}
	CopyMemory(MappedResource.pData, &m_LightObj, sizeof(cbLightObject));

	CUtilDX11::UnMap(pBuffer);
}

//EOF
