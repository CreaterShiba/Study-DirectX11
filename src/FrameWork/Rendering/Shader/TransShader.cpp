/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		TransShader.cpp
	@brief		ただの変換シェーダ
	@author		ta.Ishii
	@date		2012/3/4 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "TransShader.h"
#include "FrameWork/Utility/UtilMacro.h"
#include "FrameWork/DXUtil/UtilDX11.h"

using namespace DirectX;

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CTransShader::CTransShader()
: mpNeverBuffer(nullptr)
, mpEveryFrameBuffer(nullptr)
, mpEveryObjectBuffer(nullptr)
{
	m_eShaderID = CShaderManager::eSHADERID_TRANSPALENT;
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CTransShader::~CTransShader()
{
	SAFE_RELEASE( mpNeverBuffer );
	SAFE_RELEASE( mpEveryFrameBuffer );
	SAFE_RELEASE( mpEveryObjectBuffer );
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダーロード
*/
//---------------------------------------------------------------------------
HRESULT CTransShader::LoadShader()
{
	HRESULT hr = S_OK;
	
	//入力レイアウトの設定
	u32 nNormalOffset, nTexOffset;
	//nColorOffset = sizeof(NSVector3);
	nTexOffset = sizeof(NSVector3);
	nNormalOffset = sizeof(NSVector3) + sizeof(NSVector2);
	//nTangent = sizeof(NSVector3) + sizeof(NSVector3) + sizeof(NSVector2);

	D3D11_INPUT_ELEMENT_DESC layout[] = 
	{
		{"POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,				D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD"	, 0, DXGI_FORMAT_R32G32_FLOAT,		0, nTexOffset,		D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, nNormalOffset,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		//{"TANGENT"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, nTangent,		D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	UINT LayOutSize = _countof( layout );

	//頂点シェーダ生成の際は、
	//IAに入力されるデータと頂点シェーダの入力の整合性が取れているか、入力レイアウトを生成する

	//シェーダファイルのコンパイル
#if defined(SHADER_PRE_COMPILE)
	CreateShader(_T("Debug/SimpleShader_VS.cso"), CShaderManager::eSHADERTYPE_VERTEX, layout, LayOutSize);
	CreateShader(_T("Debug/SimpleShader_PS.cso"), CShaderManager::eSHADERTYPE_PIXEL);
#else
	CompileShaderFromFile( _T("sh/SimpleShader_VS.hlsl"), "VS", "vs_4_0", CShaderManager::eSHADERTYPE_VERTEX, layout, LayOutSize);
	CompileShaderFromFile( _T("sh/SimpleShader_PS.hlsl"), "PS", "ps_4_0", CShaderManager::eSHADERTYPE_PIXEL);
#endif
	//定数バッファの作成
	CUtilDX11::CreateConstantBuffer( sizeof( cbNeverChanges ), &mpNeverBuffer);
	CUtilDX11::CreateConstantBuffer( sizeof( cbChangesEveryFrame ), &mpEveryFrameBuffer);
	CUtilDX11::CreateConstantBuffer( sizeof( cbChangesEveryObject ), &mpEveryObjectBuffer);

	return hr;
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダオン
*/
//---------------------------------------------------------------------------
bool CTransShader::ShaderOn()
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
		mpEveryFrameBuffer, 
		mpEveryObjectBuffer
	};

	DXMng* pDX = DXMng::GetInstance();
	pDX->GetConDevice().VSSetConstantBuffers( 0, CONBUFFERNUM, pBuffer );
	pDX->GetConDevice().PSSetConstantBuffers( 0, CONBUFFERNUM, pBuffer );
	//pDX->GetConDevice().GSSetConstantBuffers( 0, CONBUFFERNUM, pBuffer );

	return true;
}


//---------------------------------------------------------------------------
/*!
	@brief	射影行列設定
	@param[in]	ProjMat	射影行列
*/
//---------------------------------------------------------------------------
void CTransShader::SetProjection( NSMatrix& ProjMat )
{
	//コンパイル時、行列が列優先になっているので転置する
	NSMatrix Mat = XMMatrixTranspose( ProjMat );

	DXMng* pDX = DXMng::GetInstance();
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HRESULT hr = pDX->GetConDevice().Map(
		mpNeverBuffer,				// マップ（開く）するリソース
		0,							// サブリソースのインデックス番号
		D3D11_MAP_WRITE_DISCARD,	// 書き込み方式
		0,							// ?
		&MappedResource);			// データ書き込み先ポインタ
	if( FAILED( hr ) ){
		return;
	}
	//開いたリソースにメモリ内容をコピーする
	CopyMemory( MappedResource.pData, &Mat, sizeof( cbNeverChanges ));
	
	//リソースを閉じる(Unmap)
	pDX->GetConDevice().Unmap( mpNeverBuffer, 0);
}

//---------------------------------------------------------------------------
/*!
	@brief		ビュー行列設定
	@param[in]	ViewMat	ビュー行列
*/
//---------------------------------------------------------------------------
void CTransShader::SetView( NSMatrix& ViewMat )
{
	NSMatrix Mat = XMMatrixTranspose(ViewMat);
	
	DXMng* pDX = DXMng::GetInstance();
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HRESULT hr = pDX->GetConDevice().Map( 
		mpEveryFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource
		);
	if( FAILED( hr ) ){
		return;
	}
	CopyMemory( MappedResource.pData, &Mat, sizeof( cbChangesEveryFrame ));
	pDX->GetConDevice().Unmap( mpEveryFrameBuffer, 0);
}
//---------------------------------------------------------------------------
/*!
	@brief		ローカル行列設定
	@param[in]	Local	ローカル行列
*/
//---------------------------------------------------------------------------
void CTransShader::SetLocal( NSMatrix& Local )
{
	m_cbEveryChange.World = XMMatrixTranspose(Local);

	DXMng* pDX = DXMng::GetInstance();
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HRESULT hr = pDX->GetConDevice().Map( 
		mpEveryObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource
		);
	if( FAILED( hr ) ){
		return;
	}
	CopyMemory( MappedResource.pData, &m_cbEveryChange, sizeof( cbChangesEveryObject ));
	pDX->GetConDevice().Unmap( mpEveryObjectBuffer, 0);
}

//---------------------------------------------------------------------------
/*!
	@brief	まとめてセット
*/
//---------------------------------------------------------------------------
void CTransShader::SetLocalWLPWLPT
( 
	const NSMatrix& mW, 
	const NSMatrix& mW2, 
	const NSMatrix& mW3,
	const NSVector3& vLightPos,
	const NSVector3& vEyePos,
	const NSMatrix& wvp
)
{
	DXMng* pDX = DXMng::GetInstance();
	D3D11_MAPPED_SUBRESOURCE MappedResource;

#if 1

	cbChangesEveryObject cb;

	if( SUCCEEDED( pDX->GetConDevice().Map( mpEveryObjectBuffer, 0, 
		D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ) ) )
	{
		//ワールド行列を渡す
		cb.World = XMMatrixTranspose( mW );

		//ワールド、ライトビュー、射影行列　を渡す
		cb.WLP = XMMatrixTranspose( mW2 );

		//ワールド、ライトビュー、射影行列、テクスチャ行列　を渡す
		cb.WLPT = XMMatrixTranspose( mW3 );

		//ライト座標(現状w値には環境光の強さを。)
		cb.vLightPos = XMVectorSet( vLightPos.x, vLightPos.y, vLightPos.z, 0.7f );
		
		cb.wvp = XMMatrixTranspose( wvp );

		//視点
		cb.vEyePos = XMVectorSet( vEyePos.x, vEyePos.y, vEyePos.z, 0.0f );

		//CopyMemory( MappedResource.pData, &m_cbEveryChange, sizeof( cbChangesEveryObject ));
		memcpy_s( MappedResource.pData, MappedResource.RowPitch, (void*)(&cb), sizeof( cb ) );
		pDX->GetConDevice().Unmap( mpEveryObjectBuffer, 0 );
	}

#else

	if( SUCCEEDED( pDX->GetConDevice().Map( mpEveryObjectBuffer, 0, 
		D3D11_MAP_READ_WRITE, 0, &MappedResource ) ) )
	{
		//ワールド行列を渡す
		m_cbEveryChange.World = XMMatrixTranspose( mW );

		//ワールド、ライトビュー、射影行列　を渡す
		m_cbEveryChange.WLP = XMMatrixTranspose( mW2 );

		//ワールド、ライトビュー、射影行列、テクスチャ行列　を渡す
		m_cbEveryChange.WLPT = XMMatrixTranspose( mW3 );

		//ライト座標
		//m_cbEveryChange.vLightPos.x = 1.0f;
		//m_cbEveryChange.vLightPos.y = 1.0f;
		//m_cbEveryChange.vLightPos.z = 1.0f;
		//m_cbEveryChange.vLightPos.w = 1.0f;

		//CopyMemory( MappedResource.pData, &m_cbEveryChange, sizeof( cbChangesEveryObject ));
		memcpy_s( MappedResource.pData, MappedResource.RowPitch, (void*)(&m_cbEveryChange), sizeof( m_cbEveryChange ) );
		pDX->GetConDevice().Unmap( mpEveryObjectBuffer, 0 );
	}

#endif

}

//---------------------------------------------------------------------------
/*!
	@brief	視線設定
*/
//---------------------------------------------------------------------------
void CTransShader::SetEyePos( const NSVector3& vEyePos )
{
#if 0
	D3D11_MAPPED_SUBRESOURCE pData;
	m_cbEveryChange.vEyePos = vEyePos;

	DXMng* pDX = DXMng::GetInstance();
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HRESULT hr = pDX->GetConDevice().Map( 
		mpEveryObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource
		);
	if( FAILED( hr ) ){
		return;
	}
	memcpy_s( pData.pData, pData.RowPitch, (void*)(&m_cbEveryChange), sizeof( m_cbEveryChange ) );
	pDX->GetConDevice().Unmap( mpEveryObjectBuffer, 0);
#endif
	NS_UNUSED(vEyePos);

}

//---------------------------------------------------------------------------
/*!
	@brief	WVP
*/
//---------------------------------------------------------------------------
void CTransShader::SetWVP( NSMatrix& wvpMat )
{
	NSMatrix Mat = XMMatrixTranspose( wvpMat );

	DXMng* pDX = DXMng::GetInstance();
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HRESULT hr = pDX->GetConDevice().Map( 
		mpEveryObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource
		);
	if( FAILED( hr ) ){
		return;
	}
	CopyMemory( MappedResource.pData, &m_cbEveryChange, sizeof( cbChangesEveryObject ));
	pDX->GetConDevice().Unmap( mpEveryObjectBuffer, 0);
}

//EOF
