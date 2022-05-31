/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		GeometryShader.cpp
	@brief		ジオメトリシェーダテスト
	@author		ta.Ishii
	@date		2013/1/22 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "GeometryShader.h"
#include "FrameWork/DXUtil/UtilDX11.h"

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CGeometryShader::CGeometryShader()
: mpMatrixBuffer( nullptr )
, mpLightVecBuffer( nullptr )
{
	m_eShaderID = CShaderManager::eSHADERID_GEOMETRYTEST;
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CGeometryShader::~CGeometryShader()
{
	SAFE_RELEASE( mpLightVecBuffer );
	SAFE_RELEASE( mpMatrixBuffer );
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダーロード
*/
//---------------------------------------------------------------------------
HRESULT CGeometryShader::LoadShader()
{
	HRESULT hr = S_OK;

	//入力レイアウトの設定
	u32 nNomOffset, nTexOffset;
	nNomOffset = sizeof(NSVector3);
	nTexOffset = sizeof(NSVector2) + sizeof(NSVector3);

	D3D11_INPUT_ELEMENT_DESC layout[] = 
	{
		{"POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,				D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, nNomOffset,		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD"	, 0, DXGI_FORMAT_R32G32_FLOAT,		0, nTexOffset,		D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	//UINT LayOutSize = _countof( layout );

	//シェーダファイルのコンパイル
#if defined(SHADER_PRE_COMPILE)

#else
	CompileShaderFromFile( _T("sh/geomtest.sh"), "VS_Main", "vs_4_0", CShaderManager::eSHADERTYPE_VERTEX, layout, LayOutSize );
	CompileShaderFromFile( _T("sh/geomtest.sh"), "PS_Main", "ps_4_0", CShaderManager::eSHADERTYPE_PIXEL );
	CompileShaderFromFile( _T("sh/geomtest.sh"), "GS_Main", "gs_5_0", CShaderManager::eSHADERTYPE_GEOMETRY );
#endif

	//定数バッファの作成
	CUtilDX11::CreateConstantBuffer( sizeof( cbMatrixBuffer ), &mpMatrixBuffer);
	CUtilDX11::CreateConstantBuffer( sizeof( cbLightVec ), &mpLightVecBuffer);

	return hr;
}
//---------------------------------------------------------------------------
/*!
	@brief	シェーダオン
*/
//---------------------------------------------------------------------------
bool CGeometryShader::ShaderOn()
{
	//シェーダをパイプラインに設定
	if( !CShaderBase::ShaderOn() )
	{
		return false;
	}

	//シェーダ定数スロット設定
	ID3D11Buffer* pBuffer[ CONBUFFERNUM ] = 
	{
		mpMatrixBuffer, 
		mpLightVecBuffer
	};

	DXMng* pDX = DXMng::GetInstance();
	pDX->GetConDevice().VSSetConstantBuffers( 0, CONBUFFERNUM, pBuffer );
	pDX->GetConDevice().PSSetConstantBuffers( 0, CONBUFFERNUM, pBuffer );
	pDX->GetConDevice().GSSetConstantBuffers( 0, CONBUFFERNUM, pBuffer );

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	WVP行列設定
*/
//---------------------------------------------------------------------------
void CGeometryShader::SetWVP( NSMatrix& WVPMat )
{
	NSMatrix Mat = XMMatrixTranspose( WVPMat );

	ID3D11Buffer* pBuffer = mpMatrixBuffer;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HRESULT hr = CUtilDX11::Map( pBuffer, D3D11_MAP_WRITE_DISCARD, &MappedResource);
	if( FAILED( hr ) )
	{
		return;
	}

	CopyMemory( MappedResource.pData, &Mat, sizeof( cbMatrixBuffer ));

	CUtilDX11::UnMap( pBuffer );
}

//---------------------------------------------------------------------------
/*!
	@brief	ライトベクトル設定
*/
//---------------------------------------------------------------------------
void CGeometryShader::SetVecLight( NSVector4& vecLight )
{
	ID3D11Buffer* pBuffer = mpLightVecBuffer;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HRESULT hr = CUtilDX11::Map( pBuffer, D3D11_MAP_WRITE_DISCARD, &MappedResource);
	if( FAILED( hr ) )
	{
		return;
	}

	CopyMemory( MappedResource.pData, &vecLight, sizeof( cbMatrixBuffer ));

	CUtilDX11::UnMap( pBuffer );
}

//EOF
