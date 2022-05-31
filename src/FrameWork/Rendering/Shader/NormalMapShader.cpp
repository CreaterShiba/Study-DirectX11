/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		NormalMapShader.cpp
	@brief		法線書き込みシェーダ
	@author		ta.Ishii
	@date		2013/4/22 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "NormalMapShader.h"
#include "FrameWork/DXUtil/UtilDX11.h"

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CNormalMapShader::CNormalMapShader()
: mpEveryFrameBuffer( nullptr )
{
	m_eShaderID = CShaderManager::eSHADERID_NORMALMAP;
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CNormalMapShader::~CNormalMapShader()
{
	SAFE_RELEASE( mpEveryFrameBuffer );
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダオン
*/
//---------------------------------------------------------------------------
bool CNormalMapShader::ShaderOn()
{
	//シェーダをパイプラインにセット
	if( !CShaderBase::ShaderOn() )
	{
		return false;
	}

	//シェーダ定数スロット設定
	ID3D11Buffer* pBuffer[CONBUFFERNUM] = 
	{
		mpEveryFrameBuffer, 
	};

	DXMng* pDX = DXMng::GetInstance();
	pDX->GetConDevice().VSSetConstantBuffers( 0, CONBUFFERNUM, pBuffer );
	pDX->GetConDevice().PSSetConstantBuffers( 0, CONBUFFERNUM, pBuffer );

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダロード
*/
//---------------------------------------------------------------------------
HRESULT CNormalMapShader::LoadShader()
{
	HRESULT hr = S_OK;

	//入力レイアウトの設定
	u32 nNormalOffset, nTexOffset;
	//nColorOffset = sizeof(NSVector3);
	nNormalOffset = sizeof(NSVector3)/* + sizeof(NSVector3)*/;
	nTexOffset = sizeof(NSVector3) + sizeof(NSVector3)/* + sizeof(NSVector3)*/;

	D3D11_INPUT_ELEMENT_DESC layout[] = 
	{
		{"POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,				D3D11_INPUT_PER_VERTEX_DATA, 0},
		//{"COLOR"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, nColorOffset,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, nNormalOffset,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXTURE"	, 0, DXGI_FORMAT_R32G32_FLOAT,		0, nTexOffset,		D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
/*
	//入力レイアウトの設定
	//入力レイアウトはシェーダごとじゃなくて、頂点バッファごと？
	s32 nPosOff1, nPosOff2;
	nPosOff1 = sizeof(NSVector4);
	nPosOff2 = sizeof(NSVector4) + sizeof(NSVector4);

	D3D11_INPUT_ELEMENT_DESC layout[] = 
	{
		{"POSITION" , 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"POSITION"	, 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 16,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
*/

	//頂点シェーダ生成の際は、
	//IAに入力されるデータと頂点シェーダの入力の整合性が取れているか、入力レイアウトを生成する

	//シェーダファイルのコンパイル
#if defined(SHADER_PRE_COMPILE)

#else
	UINT LayOutSize = _countof(layout);
	CompileShaderFromFile( _T("sh/NormalMap.hlsl"), "VS_Nom", "vs_4_0", CShaderManager::eSHADERTYPE_VERTEX, layout, LayOutSize );
	CompileShaderFromFile( _T("sh/NormalMap.hlsl"), "PS_Nom", "ps_4_0", CShaderManager::eSHADERTYPE_PIXEL);
#endif

	//定数バッファの作成
	CUtilDX11::CreateConstantBuffer( sizeof( cbChangesEveryObject ), &mpEveryFrameBuffer);

	return hr;
}

//---------------------------------------------------------------------------
/*!
	@brief		行列設定
	@param[in]	mWVP	WVP行列
*/
//---------------------------------------------------------------------------
void CNormalMapShader::SetWVP( const NSMatrix& mWVP )
{
	//コンパイル時、行列が列優先になっているので転置する
	NSMatrix Mat = XMMatrixTranspose(mWVP);

	DXMng* pDX = DXMng::GetInstance();
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HRESULT hr = pDX->GetConDevice().Map(
		mpEveryFrameBuffer,				// マップ（開く）するリソース
		0,								// サブリソースのインデックス番号
		D3D11_MAP_WRITE_DISCARD,		// 書き込み方式
		0,								// ?
		&MappedResource);			// データ書き込み先ポインタ
	if( FAILED( hr ) ){
		return;
	}
	//開いたリソースにメモリ内容をコピーする
	CopyMemory( MappedResource.pData, &Mat, sizeof( cbChangesEveryObject ));

	//リソースを閉じる(Unmap)
	pDX->GetConDevice().Unmap( mpEveryFrameBuffer, 0);
}


//EOF
