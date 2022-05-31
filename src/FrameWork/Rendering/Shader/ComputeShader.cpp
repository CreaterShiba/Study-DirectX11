
/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		ComputeShader.cpp
	@brief		Computeシェーダテスト
	@author		ta.Ishii
	@date		2012/2/29 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "ComputeShader.h"

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CComputeShader::CComputeShader() :
m_pVertexBuffer( nullptr )
{

}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CComputeShader::~CComputeShader()
{
	SAFE_RELEASE( m_pVertexBuffer );
}

//---------------------------------------------------------------------------
/*!
	@brief	初期化
*/
//---------------------------------------------------------------------------
bool CComputeShader::Init()
{
	CShaderBase::Init();

	//とりあえずビュー作ってみるか

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory( &uavDesc, sizeof( uavDesc ) );

	uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;			//D3D11_BUFFER_UAV_FLAG_RAWを使うならこれでしかできない
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;	//バッファとして使用する
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = 0;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;

	DXMng* pDX = DXMng::GetInstance();
	NS_ASSERT( pDX );

	ID3D11Buffer* pBuffer = nullptr;
	ID3D11UnorderedAccessView* pView = nullptr;
	pDX->GetDevice().CreateUnorderedAccessView( pBuffer, &uavDesc, &pView );

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダオン
	@param
*/
//---------------------------------------------------------------------------
bool CComputeShader::ShaderOn()
{
	DXMng* pDX = DXMng::GetInstance();

	//シェーダをパイプラインに設定
	if( !CShaderBase::ShaderOn() )
	{
		return false;
	}

	NS_UNUSED( pDX );

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダーロード
	@param
*/
//---------------------------------------------------------------------------
HRESULT CComputeShader::LoadShader()
{
	HRESULT hr = S_OK;

	return hr;
}

//EOF