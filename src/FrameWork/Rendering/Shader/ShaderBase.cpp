/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		ShaderBase.h
	@brief		シェーダーベースクラス
	@author		shiba
	@date		2012/3/4 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "ShaderBase.h"
#include "d3dcompiler.h"

#pragma comment(lib, "d3dcompiler.lib")

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CShaderBase::CShaderBase()
: m_eShaderID( CShaderManager::eSHADERID_END )
#if defined( _DEBUG )
, m_DebugShaderParam(nullptr)
#endif
, m_pVertexShader(nullptr)
, m_pPixelShader(nullptr)
, m_pHullShader(nullptr)
, m_pDomainShader(nullptr)
, m_pGeometryShader(nullptr)
, m_pInputLayout(nullptr)
{

}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CShaderBase::~CShaderBase()
{
	Term();
}

//---------------------------------------------------------------------------
/*!
	@brief	初期化
*/
//---------------------------------------------------------------------------
bool CShaderBase::Init()
{
	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダをパイプラインにセット
*/
//---------------------------------------------------------------------------
bool CShaderBase::ShaderOn()
{
	DXMng* pcDX = DXMng::GetInstance();

	// 頂点入力レイアウトを設定
	if( m_pInputLayout )
	{
		pcDX->GetConDevice().IASetInputLayout( m_pInputLayout );
	}
	//頂点シェーダ開始
	if( m_pVertexShader )
	{
		pcDX->GetConDevice().VSSetShader( m_pVertexShader, nullptr, 0 );
	}
	//ピクセルシェーダ開始
	if( m_pPixelShader)
	{
		pcDX->GetConDevice().PSSetShader( m_pPixelShader, nullptr, 0 );
	}
	//ハルシェーダ開始
	if( m_pHullShader )
	{
		pcDX->GetConDevice().HSSetShader( m_pHullShader, nullptr, 0 );
	}
	//ドメインシェーダ開始
	if( m_pDomainShader  )
	{
		pcDX->GetConDevice().DSSetShader( m_pDomainShader, nullptr, 0 );
	}
	//ジオメトリシェーダ開始
	if( m_pGeometryShader)
	{
		pcDX->GetConDevice().GSSetShader( m_pGeometryShader, nullptr, 0 );
	}

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	シェーダオフ
*/
//---------------------------------------------------------------------------
bool CShaderBase::ShaderOff()
{
	DXMng* pcDX = DXMng::GetInstance();

	pcDX->GetConDevice().VSSetShader(nullptr, nullptr, 0 );
	pcDX->GetConDevice().PSSetShader(nullptr, nullptr, 0 );
	pcDX->GetConDevice().HSSetShader(nullptr, nullptr, 0 );
	pcDX->GetConDevice().DSSetShader(nullptr, nullptr, 0 );
	pcDX->GetConDevice().GSSetShader(nullptr, nullptr, 0 );

	return true;
}

#if defined(SHADER_PRE_COMPILE)

//---------------------------------------------------------------------------
/*!
	@brief	シェーダデータの作成
	@param[in]	pFileName		ファイル名
	@param[in]	eShaderType		シェーダータイプ
	@param[in]	Layout			頂点シェーダ用レイアウト
	@param[in]	LayoutSize		頂点シェーダ用レイアウトサイズ
*/
//---------------------------------------------------------------------------
bool CShaderBase::CreateShader(
	const TCHAR* pFileName, 
	CShaderManager::E_SHADERTYPE eShaderType,
	D3D11_INPUT_ELEMENT_DESC Layout[] /*= 0*/,
	UINT LayoutSize /*= 0*/)
{
	bool bSuccess = true;

	//ファイル読み込み
	FILE* fp = _tfopen(pFileName, _T("rb"));
	if (!fp) 
	{
		return false;
	}

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	unsigned char* cso_data = new unsigned char[cso_sz];
	fread(cso_data, cso_sz, 1, fp);
	fclose(fp);

	//cso_dataから、シェーダオブジェクトの作成をする

	//バイトコードからシェーダオブジェクトのインターフェイスを作成する
	HRESULT hr = S_FALSE;
	DXMng* pDX = DXMng::GetInstance();
	switch (eShaderType)
	{
		case CShaderManager::eSHADERTYPE_VERTEX:
		{
			hr = pDX->GetDevice().CreateVertexShader(
				cso_data,							//バイトコードポインタ
				cso_sz,								//バイトコード長さ
				nullptr,
				&m_pVertexShader);

			if (!FAILED(hr))
			{
				HRESULT retLayout = pDX->GetDevice().CreateInputLayout(
					Layout, LayoutSize,
					cso_data,
					cso_sz,
					&m_pInputLayout);

				//入力レイアウトの生成
				if (retLayout == E_INVALIDARG || FAILED(retLayout))
				{
					bSuccess = false;
					NS_Report(_T("Error Shader !!! InputLayoutFailed[%s]\n"), pFileName);
				}
			}
			else
			{
				bSuccess = false;
				NS_Report(_T("Error Shader !!! CreateVertexShader[%s]\n"), pFileName);
			}
		}
		break;

		case CShaderManager::eSHADERTYPE_PIXEL:
			if (FAILED(pDX->GetDevice().CreatePixelShader(
				cso_data,							//バイトコードポインタ
				cso_sz,								//バイトコード長さ
				nullptr,
				&m_pPixelShader)))
			{
				bSuccess = false;
				NS_Report(_T("Error Shader !!! CreatePixelShader[%s]\n"), pFileName);
			}
			break;

		case CShaderManager::eSHADERTYPE_GEOMETRY:
			if (FAILED(pDX->GetDevice().CreateGeometryShader(
				cso_data,							//バイトコードポインタ
				cso_sz,								//バイトコード長さ
				nullptr,
				&m_pGeometryShader)))
			{
				bSuccess = false;
				NS_Report(_T("Error Shader !!! CreateGeometryShader[%s]\n"), pFileName);
			}
			break;

		case CShaderManager::eSHADERTYPE_HAL:
			if (FAILED(pDX->GetDevice().CreateHullShader(
				cso_data,							//バイトコードポインタ
				cso_sz,								//バイトコード長さ
				nullptr,
				&m_pHullShader)))
			{
				bSuccess = false;
				NS_Report(_T("Error Shader !!! CreateHalShader[%s]\n"), pFileName);
			}
			break;

		case CShaderManager::eSHADERTYPE_DOMAIN:
			if (FAILED(pDX->GetDevice().CreateDomainShader(
				cso_data,							//バイトコードポインタ
				cso_sz,								//バイトコード長さ
				nullptr,
				&m_pDomainShader)))
			{
				bSuccess = false;
				NS_Report(_T("Error Shader !!! CreateDomainShader[%s]\n"), pFileName);
			}
			break;

		default:
			bSuccess = false;
			NS_Report(_T("Not Shader ID"));
			break;
	}

	//データ削除
	SAFE_DELETEA(cso_data);

	return bSuccess;
}

#else


//---------------------------------------------------------------------------
/*!
	@brief	シェーダコンパイル
*/
//---------------------------------------------------------------------------
bool CShaderBase::CompileShaderFromFile
( 
	const TCHAR* pFileName, 
	const TCHAR* pEntryPoint, 
	const TCHAR* pShaderModel,
	CShaderManager::E_SHADERTYPE ShaderType,
	D3D11_INPUT_ELEMENT_DESC Layout[],
	UINT LayoutSize
)
{	
	bool bSuccess = true;
#if true
	HRESULT hr = S_OK;

	//シェーダコンパイル
	ID3DBlob* pBlobShader = nullptr;
	ID3DBlob* pErrorBrob = nullptr;

	hr = D3DCompile(
		pFileName,								//シェーダファイル名
		nullptr,									//マクロ定義なし
		nullptr,									//インクルードファイル定義なし
		pEntryPoint,							//シェーダの関数名
		pShaderModel,							//シェーダモデル
#if defined( _DEBUG )
		D3D10_SHADER_DEBUG |						//デバッグ
		D3D10_SHADER_SKIP_OPTIMIZATION	|			//最適化をやめる
		D3D10_SHADER_ENABLE_STRICTNESS,				//旧文法の禁止
		//D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR,	//列配置優先（デフォルト）
#endif
#if defined( _RELEASE )
		D3D10_SHADER_OPTIMIZATION_LEVEL3,		//シェーダ最適化
#endif
		0,										//エフェクトファイルコンパイルオプションなし
		nullptr,									//すぐにコンパイルして、関数を出る
		&pBlobShader,							//コンパイルされたバイトコード
		&pErrorBrob,							//エラー情報
		nullptr);

	static TCHAR str[2048] = { 0 };
	if( FAILED( hr ) )
	{
		//シェーダコンパイルの失敗
		LPVOID l_pError = pErrorBrob->GetBufferPointer();
		//swprintf_s(  str, sizeof( str ), _T("ShaderCompileError\n%s"), (char*)l_pError );
		sprintf_s( str, sizeof( str ), _T("ShaderCompileError\n%s"), (char*)l_pError );
		bSuccess = false;
	}
	else
	{
		//シェーダコンパイル成功
		if( pErrorBrob )
		{
			LPVOID l_pError = pErrorBrob->GetBufferPointer();
			if( l_pError )
			{
				//警告は出しておいたほうがよい
				NS_Report("ShaderCompile[%s]:%s", pFileName, (char*)l_pError );
			}
		}
	}

	if( bSuccess )
	{
		//バイトコードからシェーダオブジェクトのインターフェイスを作成する
		DXMng* pDX = DXMng::GetInstance();
		switch( ShaderType )
		{
			case CShaderManager::eSHADERTYPE_VERTEX :
				{
					if( !FAILED( pDX->GetDevice().CreateVertexShader(
						pBlobShader->GetBufferPointer(),	//バイトコードポインタ
						pBlobShader->GetBufferSize(),		//バイトコード長さ
						nullptr,
						&m_pVertexShader) ) )
					{
						HRESULT retLayout = pDX->GetDevice().CreateInputLayout(
							Layout, LayoutSize, 
							pBlobShader->GetBufferPointer(), 
							pBlobShader->GetBufferSize(),
							&m_pInputLayout);
						//入力レイアウトの生成
						if( retLayout == E_INVALIDARG || FAILED( retLayout ) ) 
						{
							bSuccess = false;
							NS_Report("Error Shader !!! InputLayoutFailed[%s]\n", pFileName);
						}
					}
					else
					{
						bSuccess = false;
						NS_Report("Error Shader !!! CreateVertexShader[%s]\n", pFileName);
					}
				}
				break;
			case CShaderManager::eSHADERTYPE_PIXEL :
				if( FAILED( pDX->GetDevice().CreatePixelShader(
					pBlobShader->GetBufferPointer(),	//バイトコードポインタ
					pBlobShader->GetBufferSize(),		//バイトコード長さ
					nullptr,
					&m_pPixelShader ) ) )
				{
					bSuccess = false;
					NS_Report("Error Shader !!! CreatePixelShader[%s]\n", pFileName);
				}
				break;

			case CShaderManager::eSHADERTYPE_GEOMETRY :
				if( FAILED( pDX->GetDevice().CreateGeometryShader(
					pBlobShader->GetBufferPointer(),	//バイトコードポインタ
					pBlobShader->GetBufferSize(),		//バイトコード長さ
					nullptr,
					&m_pGeometryShader) ) )
				{
					bSuccess = false;
					NS_Report("Error Shader !!! CreateGeometryShader[%s]\n", pFileName);
				}
				break;

			case CShaderManager::eSHADERTYPE_HAL :
				if( FAILED( pDX->GetDevice().CreateHullShader(
					pBlobShader->GetBufferPointer(),	//バイトコードポインタ
					pBlobShader->GetBufferSize(),		//バイトコード長さ
					nullptr,
					&m_pHullShader) ) )
				{
					bSuccess = false;
					NS_Report("Error Shader !!! CreateHalShader[%s]\n", pFileName);
				}
				break;

			case CShaderManager::eSHADERTYPE_DOMAIN :
				if( FAILED( pDX->GetDevice().CreateDomainShader(
					pBlobShader->GetBufferPointer(),	//バイトコードポインタ
					pBlobShader->GetBufferSize(),		//バイトコード長さ
					nullptr,
					&m_pDomainShader) ) )
				{
					bSuccess = false;
					NS_Report("Error Shader !!! CreateDomainShader[%s]\n", pFileName);
				}
				break;

			default:
				bSuccess = false;
				NS_Report(_T("Not Shader ID"));
				break;
		}
	}

#if 0
#if defined(_DEBUG)
	if( pBlobShader )
	{
		if( FAILED( D3DReflect( pBlobShader->GetBufferPointer(), pBlobShader->GetBufferSize(),
			IID_ID3D11ShaderReflection, (void**)&m_DebugShaderParam ) ) )
		{
			NS_Report("Error Create ShaderRefrect!!! [%s]\n", pFileName );
		}
		else
		{
			D3D11_SHADER_DESC desc;
			m_DebugShaderParam->GetDesc( &desc );
		}
	}

#endif
#endif

	SAFE_RELEASE( pBlobShader );
	SAFE_RELEASE( pErrorBrob );

	if( bSuccess )
	{
		NS_Report( _T("CompileShaderFromFile::OK!::%s:%s:\n"), pFileName, pEntryPoint );
	}
#endif

	return bSuccess;
}
#endif

//---------------------------------------------------------------------------
/*!
	@brief	破棄
*/
//---------------------------------------------------------------------------
void CShaderBase::Term()
{
#if defined( _DEBUG )
	SAFE_RELEASE( m_DebugShaderParam );
#endif

	SAFE_RELEASE( m_pInputLayout );
	SAFE_RELEASE( m_pGeometryShader );
	SAFE_RELEASE( m_pDomainShader );
	SAFE_RELEASE( m_pHullShader );
	SAFE_RELEASE( m_pPixelShader );
	SAFE_RELEASE( m_pVertexShader );
}

//EOF
