/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		ShaderInstance.cpp
	@brief		インスタンス描画
	@author		ta.Ishii
	@date		2013/5/19 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "ShaderInstance.h"
#include "Framework/DXUtil/UtilDX11.h"

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CShaderInstance::CShaderInstance()
: mpEveryObjectBuffer( nullptr )
, mpMatrixBuffer( nullptr )
{
	m_eShaderID = CShaderManager::eSHADERID_INSTANCE;
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CShaderInstance::~CShaderInstance()
{
	SAFE_RELEASE( mpMatrixBuffer );
	SAFE_RELEASE( mpEveryObjectBuffer );
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダーロード
*/
//---------------------------------------------------------------------------
HRESULT CShaderInstance::LoadShader()
{
	HRESULT hr = S_OK;
	
	//入力レイアウトの設定
	u32 nTexOffset, nNormal;
	nTexOffset = sizeof(NSVector3);
	nNormal = sizeof(NSVector3) + sizeof(NSVector2);

	D3D11_INPUT_ELEMENT_DESC layout[] = 
	{
		{"POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,				D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXTURE"	, 0, DXGI_FORMAT_R32G32_FLOAT,		0, nTexOffset,		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, nNormal,			D3D11_INPUT_PER_VERTEX_DATA, 0},
		
		//インスタンシング用設定
		{ "MATRIX",   0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,  0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX",   1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX",   2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "MATRIX",   3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};

	//シェーダファイルのコンパイル

#if defined(SHADER_PRE_COMPILE)
#else
	UINT LayOutSize = _countof(layout);
	CompileShaderFromFile( _T("sh/instancing.sh"), "VS_Main", "vs_5_0", CShaderManager::eSHADERTYPE_VERTEX, layout, LayOutSize);
	CompileShaderFromFile( _T("sh/instancing.sh"), "PS_Main", "ps_5_0", CShaderManager::eSHADERTYPE_PIXEL);
#endif
	//定数バッファの作成
	CUtilDX11::CreateConstantBuffer( sizeof( cbChangesEveryObject ), &mpEveryObjectBuffer);
	CUtilDX11::CreateConstantBuffer( sizeof( NSMatrix )*INSTANCE_NUM, &mpMatrixBuffer );

	return hr;
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダオン
*/
//---------------------------------------------------------------------------
bool CShaderInstance::ShaderOn()
{
	//シェーダをパイプラインに設定
	if( !CShaderBase::ShaderOn() )
	{
		return false;
	}

	//シェーダ定数スロット設定
	ID3D11Buffer* pBuffer[CONBUFFERNUM] = 
	{
		mpEveryObjectBuffer
	};

	DXMng* pDX = DXMng::GetInstance();
	pDX->GetConDevice().VSSetConstantBuffers( 0, CONBUFFERNUM, pBuffer );
	pDX->GetConDevice().PSSetConstantBuffers( 0, CONBUFFERNUM, pBuffer );

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	行列設定
*/
//---------------------------------------------------------------------------
void CShaderInstance::setMatrixs( 
	const NSMatrix* pMatrix, 
	const NSMatrix& view,  
	const NSMatrix& proj )
{
	if( pMatrix )
	{
		D3D11_MAPPED_SUBRESOURCE pData;

		DXMng* pcDirect = DXMng::GetInstance();
		NS_ASSERT( pcDirect );

		pcDirect->GetConDevice().Map( mpMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData );

		NSMatrix* m = (NSMatrix*)(pData.pData);
		for( s32 i = 0; i < INSTANCE_NUM; i++ )
		{
			NSMatrix mat = pMatrix[i] * view * proj;
			m[i] = XMMatrixTranspose(mat);
		}

		pcDirect->GetConDevice().Unmap( mpMatrixBuffer, 0 );
	}
}

//---------------------------------------------------------------------------
/*!
	@brief	ライト位置
*/
//---------------------------------------------------------------------------
void CShaderInstance::SetLightPos( const NSVector3& lightPos )
{
#if 0
	D3D11_MAPPED_SUBRESOURCE pData;
	cbChangesEveryObject cbChange;
	cbChange.lightPos = lightPos;

	DXMng* pDX = DXMng::GetInstance();
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HRESULT hr = pDX->GetConDevice().Map( 
		mpEveryObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource
		);
	if( FAILED( hr ) ){
		return;
	}
	memcpy_s( pData.pData, pData.RowPitch, (void*)(&cbChange), sizeof( cbChangesEveryObject ) );
	pDX->GetConDevice().Unmap( mpEveryObjectBuffer, 0);
#else
	NS_UNUSED(lightPos);
#endif
}

//EOF
