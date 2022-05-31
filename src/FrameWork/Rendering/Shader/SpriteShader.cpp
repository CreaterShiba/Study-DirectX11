/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		SpriteShader.cpp
	@brief		スプライトシェーダ
	@author		shiba
	@date		2013/1/14 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "SpriteShader.h"
#include "Framework/DXUtil/UtilDX11.h"

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CSpriteShader::CSpriteShader()
: mpNeverBuffer( nullptr )
, mpEveryObjectBuffer( nullptr )
{
	m_eShaderID = CShaderManager::eSHADERID_SPRITE;
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CSpriteShader::~CSpriteShader()
{
	SAFE_RELEASE( mpNeverBuffer );
	SAFE_RELEASE( mpEveryObjectBuffer );
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダーロード
*/
//---------------------------------------------------------------------------
HRESULT CSpriteShader::LoadShader()
{
	HRESULT hr = S_OK;
	
	//入力レイアウトの設定
	u32 nTexOffset;
	nTexOffset = sizeof(NSVector3);
	////nColorOffset = sizeof(NSVector3);
	//nNormal = sizeof(NSVector3) + sizeof(NSVector2);

	D3D11_INPUT_ELEMENT_DESC layout[] = 
	{
		{"POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,				D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD"	, 0, DXGI_FORMAT_R32G32_FLOAT,		0, nTexOffset,		D3D11_INPUT_PER_VERTEX_DATA, 0},
		//{"NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, nNormal,			D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	UINT LayOutSize = _countof( layout );

	CreateShader(_T("Debug/SpriteShader_VS.cso"), CShaderManager::eSHADERTYPE_VERTEX, layout, LayOutSize);
	CreateShader(_T("Debug/SpriteShader_PS.cso"), CShaderManager::eSHADERTYPE_PIXEL);

	//定数バッファの作成
	CUtilDX11::CreateConstantBuffer( sizeof( cbNeverChanges ), &mpNeverBuffer);
	CUtilDX11::CreateConstantBuffer( sizeof( cbChangesEveryObject ), &mpEveryObjectBuffer);

	return hr;
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダオン
*/
//---------------------------------------------------------------------------
bool CSpriteShader::ShaderOn()
{
	//シェーダをパイプラインに設定
	if( !CShaderBase::ShaderOn() )
	{
		return false;
	}

	//シェーダ定数スロット設定
	ID3D11Buffer* pBuffer[CONBUFFERNUM] = 
	{
		mpNeverBuffer, 
		mpEveryObjectBuffer
	};

	DXMng* pDX = DXMng::GetInstance();
	pDX->GetConDevice().VSSetConstantBuffers( 0, CONBUFFERNUM, pBuffer );
	pDX->GetConDevice().PSSetConstantBuffers( 0, CONBUFFERNUM, pBuffer );

	return true;
}


//---------------------------------------------------------------------------
/*!
	@brief	射影行列設定
	@param[in]	ProjMat	射影行列
*/
//---------------------------------------------------------------------------
void CSpriteShader::SetProjection( NSMatrix& ProjMat )
{
	m_cNeverChange.Projection = XMMatrixTranspose(ProjMat);

	ID3D11Buffer* pBuffer = mpNeverBuffer;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HRESULT hr = CUtilDX11::Map( pBuffer, D3D11_MAP_WRITE_DISCARD, &MappedResource);
	if( FAILED( hr ) )
	{
		return;
	}

	CopyMemory( MappedResource.pData, &m_cNeverChange, sizeof( cbNeverChanges ));

	CUtilDX11::UnMap( pBuffer );
}

//---------------------------------------------------------------------------
/*!
	@brief		ビュー行列設定
	@param[in]	ViewMat	ビュー行列
*/
//---------------------------------------------------------------------------
void CSpriteShader::SetView( NSMatrix& ViewMat )
{
	m_cNeverChange.View = XMMatrixTranspose(ViewMat);

	ID3D11Buffer* pBuffer = mpNeverBuffer;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HRESULT hr = CUtilDX11::Map( pBuffer, D3D11_MAP_WRITE_DISCARD, &MappedResource);
	if( FAILED( hr ) )
	{
		return;
	}

	CopyMemory( MappedResource.pData, &m_cNeverChange, sizeof( cbNeverChanges ));

	CUtilDX11::UnMap( pBuffer );
}

//---------------------------------------------------------------------------
/*!
	@brief		ローカル行列設定
	@param[in]	Local	ローカル行列
*/
//---------------------------------------------------------------------------
void CSpriteShader::SetLocal( NSMatrix& Local )
{
	NSMatrix Mat = XMMatrixTranspose(Local);

	ID3D11Buffer* pBuffer = mpEveryObjectBuffer;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HRESULT hr = CUtilDX11::Map( pBuffer, D3D11_MAP_WRITE_DISCARD, &MappedResource);
	if( FAILED( hr ) )
	{
		return;
	}

	CopyMemory( MappedResource.pData, &Mat, sizeof( cbChangesEveryObject ));

	CUtilDX11::UnMap( pBuffer );
}

//EOF
