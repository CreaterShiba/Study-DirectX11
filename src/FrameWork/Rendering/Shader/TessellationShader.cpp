
/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		TessellationShader.cpp
	@brief		テッセレーションテスト
	@author		ta.Ishii
	@date		2013/6/30 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "TessellationShader.h"
#include "FrameWork/Utility/UtilMacro.h"
#include "FrameWork/DXUtil/UtilDX11.h"

static const f32 TessFactor = 1.0f;
static const f32 InsideFactor = 1.0f;

//#define TESTEST

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CTessellationShader::CTessellationShader()
: m_pConstantBuffer( nullptr )
, m_pConstantBuffer2( nullptr )
{
	m_eShaderID = CShaderManager::eSHADERID_TESSELLATION;
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CTessellationShader::~CTessellationShader()
{
	SAFE_RELEASE( m_pConstantBuffer2 );
	SAFE_RELEASE( m_pConstantBuffer );
}

//---------------------------------------------------------------------------
/*!
	@brief	初期化
*/
//---------------------------------------------------------------------------
bool CTessellationShader::Init()
{
	CShaderBase::Init();

	return true;
}


//---------------------------------------------------------------------------
/*!
	@brief	シェーダーロード
	@param
*/
//---------------------------------------------------------------------------
HRESULT CTessellationShader::LoadShader()
{
	HRESULT hr = S_OK;

	u32 nTexOffset = sizeof(NSVector3);

	D3D11_INPUT_ELEMENT_DESC layout[] = 
	{
		{"POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,				D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD"	, 0, DXGI_FORMAT_R32G32_FLOAT,	0, nTexOffset,		D3D11_INPUT_PER_VERTEX_DATA, 0},
	};


	//頂点シェーダ生成の際は、
	//IAに入力されるデータと頂点シェーダの入力の整合性が取れているか、入力レイアウトを生成する

	//シェーダファイルのコンパイル
#if defined(SHADER_PRE_COMPILE)
#else
	UINT LayOutSize = _countof(layout);
	CompileShaderFromFile( _T("sh/tessellation.hlsl"), "VS_Main", "vs_5_0", CShaderManager::eSHADERTYPE_VERTEX, layout, LayOutSize);
	CompileShaderFromFile( _T("sh/tessellation.hlsl"), "PS_Main", "ps_5_0", CShaderManager::eSHADERTYPE_PIXEL);
#endif

#if defined( TESTEST )
	CompileShaderFromFile( _T("sh/tessellation.hlsl"), "HS_Main", "hs_5_0", CShaderManager::eSHADERTYPE_HAL);
	CompileShaderFromFile( _T("sh/tessellation.hlsl"), "DS_Main", "ds_5_0", CShaderManager::eSHADERTYPE_DOMAIN);
#endif

	//定数バッファの作成
	CUtilDX11::CreateConstantBuffer( sizeof( CBUFFER0 ), &m_pConstantBuffer );
	CUtilDX11::CreateConstantBuffer( sizeof( CBUFFER1 ), &m_pConstantBuffer2 );

	return hr;
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダオン
	@param
*/
//---------------------------------------------------------------------------
bool CTessellationShader::ShaderOn()
{

	//シェーダをパイプラインに設定
	if( !CShaderBase::ShaderOn() )
	{
		return false;
	}


	//シェーダ定数スロット設定
	ID3D11Buffer* pBuffer[2] = 
	{
		m_pConstantBuffer, 
		m_pConstantBuffer2, 
	};

	DXMng* pDX = DXMng::GetInstance();
	NS_ASSERT( pDX );
	pDX->GetConDevice().VSSetConstantBuffers( 0, 2, pBuffer );
	pDX->GetConDevice().PSSetConstantBuffers( 0, 2, pBuffer );
#if defined( TESTEST )
	pDX->GetConDevice().HSSetConstantBuffers( 1, 1, &pBuffer[1] );
	pDX->GetConDevice().DSSetConstantBuffers( 0, 1, &pBuffer[0] );
#endif

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	行列設定
*/
//---------------------------------------------------------------------------
void CTessellationShader::setWVP( const NSMatrix& wMat )
{
	NSMatrix Mat = XMMatrixTranspose( wMat );
	m_cBuffer.g_matWVP = Mat;
	CUtilDX11::GetConDevice().UpdateSubresource( m_pConstantBuffer, 0, nullptr, &m_cBuffer, 0, 0 );
}

//---------------------------------------------------------------------------
/*!
	@brief	係数設定
*/
//---------------------------------------------------------------------------
void CTessellationShader::setTessellationConf( const f32 fTes1, const f32 fTes2 )
{
	m_cBuffer2.g_fTessFactor = fTes1;
	m_cBuffer2.g_fInsideTessFactor = fTes2;
	CUtilDX11::GetConDevice().UpdateSubresource( m_pConstantBuffer2, 0, nullptr, &m_cBuffer, 0, 0 );
}

//EOF
