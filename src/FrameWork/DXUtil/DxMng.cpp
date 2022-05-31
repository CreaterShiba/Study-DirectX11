/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		DXMng.cpp
	@brief		DirectX管理
	@author		shiba
	@date		2012/02/29 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "DXMng.h"
#include "Tchar.h"

NS_SINGLETON_INSTANCE_DEFINE(DXMng);
static const DXGI_FORMAT BACK_BUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

DXMng::DXMng()
	: mpDevice()
	, mpConDevice()
	, mpSwapChain()
	, mpGIFactory()
	, mpGIAdapter()
	, m_pSamplerState(nullptr)
	, m_pBlendState(nullptr)
	, m_pRasterizerState(nullptr)
	, m_pDepthStencil(nullptr)
{
	mMultiSampleDescList.clear();
	mDisplayModeList.clear();
}

DXMng::~DXMng()
{
	SAFE_RELEASE(m_pDepthStencil);
	SAFE_RELEASE(m_pBlendState);
	SAFE_RELEASE(m_pRasterizerState);
	SAFE_RELEASE(m_pSamplerState);

	SAFE_RELEASE(mpGIAdapter);
	SAFE_RELEASE(mpGIFactory);
	if (mpSwapChain)
	{
		BOOL bIsFull = false;
		mpSwapChain->GetFullscreenState(&bIsFull, nullptr);
		if (bIsFull) {
			mpSwapChain->SetFullscreenState(FALSE, nullptr);
		}
	}
	SAFE_RELEASE(mpSwapChain);
	if (mpConDevice)
	{
		mpConDevice->ClearState();
	}

	SAFE_RELEASE(mpConDevice);
	SAFE_RELEASE(mpDevice);
}

bool DXMng::Init()
{
	bool bSuccess = true;

	return bSuccess;
}

//---------------------------------------------------------------------------
/*!
@brief	デバイスと、スワップチェインの作成
*/
//---------------------------------------------------------------------------
bool DXMng::InitDirectX(const HWND hWnd, const EWINDOWMODE eWindowMode)
{
	IDXGIDevice1* pDXGIDev = nullptr;	//DXGIオブジェクト接続クラス
	HRESULT hr;
	RECT Rect;

	// ウィンドウのクライアント領域取得
	GetClientRect(hWnd, &Rect);

	// 初期化順のテーブル
	D3D_FEATURE_LEVEL FeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,		// DirectX11.1対応GPU
		D3D_FEATURE_LEVEL_11_0,		// DirectX11.0対応GPU
		D3D_FEATURE_LEVEL_10_1,		// DirectX10.1対応GPU
		D3D_FEATURE_LEVEL_10_0,		// DirectX10.0対応GPU
		D3D_FEATURE_LEVEL_9_3,		// DirectX9/ShaderModel3対応GPU
		D3D_FEATURE_LEVEL_9_2,		// DirectX9/ShaderModel2対応GPU
		D3D_FEATURE_LEVEL_9_1,		// DirectX9/ShaderModel2対応GPU
	};

	// デバイス、コンテキストデバイスの生成
	hr = D3D11CreateDevice(
		nullptr,											// 生成するアダプター
		D3D_DRIVER_TYPE_HARDWARE,							// ハードウェアデバイス
		nullptr,											// ソフトウェアデバイスのモジュール
#if defined( DX_ERRORCODE )
		D3D11_CREATE_DEVICE_DEBUG,							// フラグ
#else
		0,
#endif
		FeatureLevels,										// デバイスの初期化順ハードウェアテーブル
		sizeof(FeatureLevels) / sizeof(D3D_FEATURE_LEVEL),	// ↑のテーブルの数
		D3D11_SDK_VERSION,									// SDKのバージョン
		&mpDevice,											// デバイス格納先
		&mFeatureLevel,										// 初期化できたデバイス格納先
		&mpConDevice);										// デバイスコンテキスト格納先

	if FAILED(hr)
	{
		MessageBox(hWnd, _T("デバイスの生成に失敗しました"), _T("D3D11CreateDevice"), MB_ICONSTOP);
		NS_ASSERT(0);
	}

	// デバイスからIDXGIDevice1を取得してそこからアダプターとファクトリーを取得
	hr = mpDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)&pDXGIDev);
	if FAILED(hr)
	{
		MessageBox(hWnd, _T("IDXGIDevice1の取得に失敗しました"), _T("QueryInterface"), MB_ICONSTOP);
		NS_ASSERT(0);
	}

	// アダプター取得
	hr = pDXGIDev->GetAdapter(&mpGIAdapter);
	SAFE_RELEASE(pDXGIDev);
	if FAILED(hr)
	{
		MessageBox(hWnd, _T("IDXGIAdapterの取得に失敗しました"), _T("IDXGIDevice1::GetAdapter"), MB_ICONSTOP);
		NS_ASSERT(0);
	}

	// ファクトリー取得
	hr = mpGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&mpGIFactory);
	if FAILED(hr)
	{
		MessageBox(hWnd, _T("IDXGIFactoryの取得に失敗しました"), _T("IDXGIAdapter::GetParent"), MB_ICONSTOP);
		NS_ASSERT(0);
	}

	// ファクトリーにALT+ENTERで自動的に画面モード切替とかの指定
	hr = mpGIFactory->MakeWindowAssociation(hWnd, 0);
	if FAILED(hr)
	{
		NS_ASSERT(0);
	}

	// MSAAの種類取得
	// 使えるマルチサンプルの種類を列挙
	DXGI_SAMPLE_DESC SampleDesc = { 1, 0 }; //マルチサンプリングのパラメータ設定
	for (u32 i = 1; i <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; i++)
	{
		UINT Quality;
		//使用できるマルチサンプリング数を調べる
		if SUCCEEDED(mpDevice->CheckMultisampleQualityLevels(BACK_BUFFER_FORMAT, i, &Quality))
		{
			if (Quality > 0)
			{
				DXGI_SAMPLE_DESC Desc = { i, Quality - 1 };
				mMultiSampleDescList.push_back(Desc);
				SampleDesc = Desc;
			}
		}
	}

	// 画面モード列挙
	IDXGIOutput* pOutput = nullptr; //アダプターの出力等を扱う
	hr = mpGIAdapter->EnumOutputs(0, &pOutput);
	if FAILED(hr)
	{
		MessageBox(hWnd, _T("IDXGIOutputの取得に失敗しました"), _T("EnumOutputs"), MB_ICONSTOP);
		assert(0);
	}

	UINT ModeCount;
	hr = pOutput->GetDisplayModeList(BACK_BUFFER_FORMAT, 0, &ModeCount, nullptr);
	if (FAILED(hr) || (ModeCount == 0))
	{
		MessageBox(hWnd, _T("画面モードの取得に失敗しました"), _T("GetDisplayModeList"), MB_ICONSTOP);
		assert(0);
	}

	mDisplayModeList.resize(ModeCount);
	pOutput->GetDisplayModeList(BACK_BUFFER_FORMAT, 0, &ModeCount, &(mDisplayModeList[0]));
	if FAILED(hr)
	{
		MessageBox(hWnd, _T("画面モードの取得に失敗しました"), _T("GetDisplayModeList"), MB_ICONSTOP);
		assert(0);
	}
	SAFE_RELEASE(pOutput);

	DXGI_MODE_DESC DisplayMode; //ディスプレイの表示モードを記述
								// 画面モード選択
	DisplayMode = mDisplayModeList[0];
	for (UINT i = 0; i < ModeCount; i++)
	{
		// リフレッシュレート60Hzのを優先的に使う
		if (mDisplayModeList[i].RefreshRate.Numerator / mDisplayModeList[i].RefreshRate.Denominator == 60)
		{
			DisplayMode = mDisplayModeList[i];
			break;
		}
	}

	// @todo 選択式にする
	// スワップチェイン用のパラメーター
	if (eWindowMode == eWINDOWMODE_SCREEN)
	{
		//ウィンドウモード設定
		mGISwapChain.BufferDesc.Width = WINDOW_WIDTH;						// バックバッファの横
		mGISwapChain.BufferDesc.Height = WINDOW_HEIGHT;						// バックバッファの縦
		mGISwapChain.Windowed = TRUE;								// ウィンドウモードで起動するか否か
	}

	if (eWindowMode == eWINDOWMODE_FULL)
	{
		//フルスクリーン設定
		mGISwapChain.BufferDesc.Width = WINDOW_WIDTH;						// バックバッファの横
		mGISwapChain.BufferDesc.Height = WINDOW_HEIGHT;						// バックバッファの縦
		mGISwapChain.Windowed = FALSE;								// ウィンドウモードで起動するか否か
	}

	mGISwapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//mGISwapChain.BufferDesc.Format			= DisplayMode.Format;				// バックバッファのフォーマット
	mGISwapChain.BufferDesc.RefreshRate = DisplayMode.RefreshRate;					// リフレッシュレート
	mGISwapChain.BufferDesc.Scaling = DisplayMode.Scaling;							// スキャンラインのパラメーター
	mGISwapChain.BufferDesc.ScanlineOrdering = DisplayMode.ScanlineOrdering;		// スキャンラインのパラメーター
	//mGISwapChain.SampleDesc						= SampleDesc;					// マルチサンプル

	mGISwapChain.SampleDesc.Count = 1;
	mGISwapChain.SampleDesc.Quality = 0;

	mGISwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;							// バックバッファのフラグ
	mGISwapChain.BufferCount = 3;														// バックバッファ数
	mGISwapChain.OutputWindow = hWnd;													// 関連付けるウィンドウのハンドル
	mGISwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;									// 画面更新の方法
	mGISwapChain.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;						// スワップチェインのフラグ
	mGISwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;	// スキャンライン
	mGISwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;						// スケーリング

	// スワップチェインをデバイスから作成する
	hr = mpGIFactory->CreateSwapChain(mpDevice, &mGISwapChain, &mpSwapChain);
	if FAILED(hr)
	{
		MessageBox(hWnd, _T("スワップチェインの生成に失敗しました"), _T("CreateSwapChain"), MB_ICONSTOP);
		assert(0);
	}

	return true;
}

//---------------------------------------------------------------------------
/*!
@brief	ブレンドステート
*/
//---------------------------------------------------------------------------
void DXMng::SetBlendState(const D3D11_BLEND_DESC& desc)
{
	SAFE_RELEASE(m_pBlendState);
	GetDevice().CreateBlendState(&desc, &m_pBlendState);

	FLOAT BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	GetConDevice().OMSetBlendState(m_pBlendState, BlendFactor, 0xffffffff);
}

//---------------------------------------------------------------------------
/*!
@brief	サンプラーステート
*/
//---------------------------------------------------------------------------
void DXMng::SetSamplerState(const D3D11_SAMPLER_DESC& desc)
{
	SAFE_RELEASE(m_pSamplerState);

	GetDevice().CreateSamplerState(&desc, &m_pSamplerState);

	GetConDevice().VSSetSamplers(0, 1, &m_pSamplerState);	//頂点シェーダサンプラー設定
	GetConDevice().GSSetSamplers(0, 1, &m_pSamplerState);	//ジオメトリシェーダサンプラー設定
	GetConDevice().PSSetSamplers(0, 1, &m_pSamplerState);	//ピクセルシェーダサンプラー設定
}

//---------------------------------------------------------------------------
/*!
@brief	ラスタライザ
*/
//---------------------------------------------------------------------------
void DXMng::SetRasterizerState(const D3D11_RASTERIZER_DESC& desc)
{
	SAFE_RELEASE(m_pRasterizerState);
	GetDevice().CreateRasterizerState(&desc, &m_pRasterizerState);
	GetConDevice().RSSetState(m_pRasterizerState);
}

//---------------------------------------------------------------------------
/*!
@brief	デプスバッファ、ステンシルバッファ周り
*/
//---------------------------------------------------------------------------
void DXMng::SetDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& desc)
{
	SAFE_RELEASE(m_pDepthStencil);
	GetDevice().CreateDepthStencilState(&desc, &m_pDepthStencil);
	GetConDevice().OMSetDepthStencilState(m_pDepthStencil, 0);
}

void DXMng::SetAllDefaultState()
{
	//デフォルトステートの設定はしておく
	SetBlendState(GetDefaultBlendDesc());
	SetSamplerState(GetDefaultSamplerDesc());
	SetRasterizerState(GetDefaultRasterizerDesc());
	SetDepthStencilState(GetDefaultDepthStencilDesc());
}

//---------------------------------------------------------------------------
/*!
@brief	起動時設定取得
*/
//---------------------------------------------------------------------------
D3D11_BLEND_DESC DXMng::GetDefaultBlendDesc() const
{
	D3D11_BLEND_DESC BlendStateDesc;
	ZeroMemory(&BlendStateDesc, sizeof(D3D11_BLEND_DESC));
	BlendStateDesc.AlphaToCoverageEnable = FALSE;
	BlendStateDesc.IndependentBlendEnable = FALSE;
	BlendStateDesc.RenderTarget[0] = CUtilDX11::GetAlignmentBlendDesc();

	return BlendStateDesc;
}

D3D11_SAMPLER_DESC DXMng::GetDefaultSamplerDesc() const
{
	//サンプラステートの設定
	D3D11_SAMPLER_DESC SampDesc;
	ZeroMemory(&SampDesc, sizeof(SampDesc));

	SampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SampDesc.AddressU = SampDesc.AddressV = SampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SampDesc.MaxAnisotropy = 1;
	SampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	SampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	SampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	SampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	SampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	SampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	SampDesc.MipLODBias = 0.0f;
	SampDesc.MaxAnisotropy = 1;
	SampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	SampDesc.BorderColor[0] = 1.0f;
	SampDesc.BorderColor[1] = 1.0f;
	SampDesc.BorderColor[2] = 1.0f;
	SampDesc.BorderColor[3] = 1.0f;
	SampDesc.MinLOD = -FLT_MAX;
	SampDesc.MaxLOD = FLT_MAX;

	return SampDesc;
}

D3D11_RASTERIZER_DESC DXMng::GetDefaultRasterizerDesc() const
{
	//ラスタライザステートオブジェクト生成
	D3D11_RASTERIZER_DESC RSDesc;
	//RSDesc.FillMode = D3D11_FILL_WIREFRAME;	//普通描画
	RSDesc.FillMode = D3D11_FILL_SOLID;		//普通描画
	RSDesc.CullMode = D3D11_CULL_BACK;		//カリングあり
	RSDesc.FrontCounterClockwise = FALSE;	//時計周りが表面
	RSDesc.DepthBias = 0;					//深度バイアス０
	RSDesc.DepthBiasClamp = 0;				//ピクセル最大深度バイアス
	RSDesc.SlopeScaledDepthBias = 0;		// 指定ピクセルのスロープに対するスカラー
	RSDesc.DepthClipEnable = FALSE;			//深度クリッピングアリ
	RSDesc.ScissorEnable = FALSE;			//シザー短径なし（描画領域を設定できる
	RSDesc.MultisampleEnable = FALSE;		//マルチサンプリングなし
	RSDesc.AntialiasedLineEnable = FALSE;	//ライン・アンチエイリアシングなし

	return RSDesc;
}

D3D11_DEPTH_STENCIL_DESC DXMng::GetDefaultDepthStencilDesc() const
{
	D3D11_DEPTH_STENCIL_DESC ddsDesc;

	ddsDesc.DepthEnable = true;
	ddsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ddsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	ddsDesc.StencilEnable = FALSE;
	ddsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	ddsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;


	/*
	// 面が表を向いている場合のステンシル・テストの設定
	DepthStencil.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;  // 維持
	DepthStencil.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;  // 維持
	DepthStencil.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;  // 維持
	DepthStencil.FrontFace.StencilFunc        = D3D11_COMPARISON_NEVER; // 常に失敗
	// 面が裏を向いている場合のステンシル・テストの設定
	DepthStencil.BackFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;   // 維持
	DepthStencil.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;   // 維持
	DepthStencil.BackFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;   // 維持
	DepthStencil.BackFace.StencilFunc        = D3D11_COMPARISON_ALWAYS; // 常に成功
	*/

	return ddsDesc;
}

//EOF
