/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		SkinTransShader.cpp
	@brief		スキンシェーダ
	@author		ta.Ishii
	@date		2012/10/30 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "SkinTransShader.h"
#include "FrameWork/Utility/UtilMacro.h"
#include "FrameWork/DXUtil/UtilDX11.h"

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CSkinTransShader::CSkinTransShader() :
mpEveryObjectBuffer( nullptr )
{
	m_eShaderID = CShaderManager::eSHADERID_SKINTRANS;
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CSkinTransShader::~CSkinTransShader()
{
	SAFE_RELEASE( mpEveryObjectBuffer );
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダロード
*/
//---------------------------------------------------------------------------
HRESULT CSkinTransShader::LoadShader()
{
	HRESULT hr = S_OK;

	//入力レイアウト設定
	//頂点カラー、法線、テクスチャUV、影響ボーンインデックス、影響率
	u32 nColorOffset, nNormalOffset, nTexOffset, nBoneWeight, nBoneIndex;
	nColorOffset = nNormalOffset = nTexOffset = nBoneIndex = nBoneWeight = 0;
	//nColorOffset = sizeof(NSVector3);
	nTexOffset = sizeof(NSVector3);
	nNormalOffset = sizeof(NSVector3) + sizeof(NSVector2);
	nBoneWeight = sizeof(NSVector3) + sizeof(NSVector2) + sizeof(NSVector3);
	nBoneIndex = sizeof(NSVector3) + sizeof(NSVector2) + sizeof(NSVector3) + sizeof(f32)*3;

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, nTexOffset, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, nNormalOffset, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, nColorOffset, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, nBoneWeight, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, nBoneIndex, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	u32 LayoutSize = _countof( layout );

	//シェーダファイルのコンパイル
#if defined(SHADER_PRE_COMPILE)
	CreateShader(_T("Debug/SkinSimpleShader_VS.cso"), CShaderManager::eSHADERTYPE_VERTEX, layout, LayoutSize);
	CreateShader(_T("Debug/SkinSimpleShader_PS.cso"), CShaderManager::eSHADERTYPE_PIXEL);
#else
	CompileShaderFromFile( _T("sh/SkinMesh.hlsl"), "VS", "vs_4_0", CShaderManager::eSHADERTYPE_VERTEX, layout, LayoutSize);
	CompileShaderFromFile( _T("sh/SkinMesh.hlsl"), "PS", "ps_4_0", CShaderManager::eSHADERTYPE_PIXEL);
#endif

	//定数バッファの作成
	CUtilDX11::CreateConstantBuffer( sizeof( cbChangesEveryObject ), &mpEveryObjectBuffer);

	return hr;
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダを有効化
*/
//---------------------------------------------------------------------------
bool CSkinTransShader::ShaderOn()
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
	pDX->GetConDevice().VSSetConstantBuffers( 0, 1, pBuffer );
	pDX->GetConDevice().PSSetConstantBuffers( 0, 1, pBuffer );

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief		ワールド行列設定
	@param[in]	pCombMat	ワールド行列
*/
//---------------------------------------------------------------------------
void CSkinTransShader::SetCombMatrix(NSMatrix *pCombMat, const s32 nMatSize )
{
	if (pCombMat == nullptr)
	{
		return;
	}

	for( s32 i = 0; i < nMatSize; ++i )
	{
		cb.World[ i ] = XMMatrixTranspose( pCombMat[i] );
	}
	
	DXMng* pDX = DXMng::GetInstance();
	D3D11_MAPPED_SUBRESOURCE pData;

	HRESULT hr = pDX->GetConDevice().Map( 
		mpEveryObjectBuffer, 0, 
		D3D11_MAP_WRITE_DISCARD, 0, &pData
		);
	if( FAILED( hr ) ){
		return;
	}

	memcpy_s( pData.pData, pData.RowPitch, (void*)(&cb), sizeof( cb ) );
	//CopyMemory( MappedResource.pData, &cb, sizeof( cbChangesEveryObject ));
	pDX->GetConDevice().Unmap( mpEveryObjectBuffer, 0);
}

//---------------------------------------------------------------------------
/*!
	@brief	射影行列設定
	@param[in]	ProjMat	射影行列
*/
//---------------------------------------------------------------------------
void CSkinTransShader::SetProjection( NSMatrix& ProjMat )
{
	//コンパイル時、行列が列優先になっているので転置する
	cb.Proj = XMMatrixTranspose( ProjMat );

	DXMng* pDX = DXMng::GetInstance();
	D3D11_MAPPED_SUBRESOURCE pData;

	HRESULT hr = pDX->GetConDevice().Map(
		mpEveryObjectBuffer,				// マップ（開く）するリソース
		0,							// サブリソースのインデックス番号
		D3D11_MAP_WRITE_DISCARD,	// 書き込み方式
		0,							// ?
		&pData);			// データ書き込み先ポインタ
	if( FAILED( hr ) ){
		return;
	}
	//開いたリソースにメモリ内容をコピーする
	memcpy_s( pData.pData, pData.RowPitch, (void*)(&cb), sizeof( cb ) );
	//CopyMemory( MappedResource.pData, &Mat, sizeof( cbChangesEveryObject ));
	
	//リソースを閉じる(Unmap)
	pDX->GetConDevice().Unmap( mpEveryObjectBuffer, 0);
}

//---------------------------------------------------------------------------
/*!
	@brief		ビュー行列設定
	@param[in]	ViewMat	ビュー行列
*/
//---------------------------------------------------------------------------
void CSkinTransShader::SetView( NSMatrix& ViewMat )
{
	cb.View =  XMMatrixTranspose( ViewMat );
	
	DXMng* pDX = DXMng::GetInstance();
	D3D11_MAPPED_SUBRESOURCE pData;

	HRESULT hr = pDX->GetConDevice().Map( 
		mpEveryObjectBuffer, 0, 
		D3D11_MAP_WRITE_DISCARD, 0, &pData
		);
	if( FAILED( hr ) ){
		return;
	}

	memcpy_s( pData.pData, pData.RowPitch, (void*)(&cb), sizeof( cb ) );
	//CopyMemory( MappedResource.pData, &Mat, sizeof( cbChangesEveryObject ));
	pDX->GetConDevice().Unmap( mpEveryObjectBuffer, 0);
}


//EOF
