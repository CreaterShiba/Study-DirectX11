/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		GameSystem.cpp
	@brief		ゲームシステムメイン
	@author		shiba
	@date		2012/3/11 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include <mmsystem.h> // timeGetTime()のため

#include "GameSystem.h"

#include "FrameWork/Win32/Window.h"
#include "FrameWork/DXUtil/DXMng.h"

#include "FrameWork/Asset/AssetManager.h"
#include "FrameWork/Asset/AssetTexture.h"

#include "FrameWork/Loader/LoaderManager.h"
#include "FrameWork/Task/TaskScheduler.h"
#include "FrameWork/Font/FontManager.h"
#include "FrameWork/Camera/CameraManager.h"
#include "FrameWork/Input/InputManager.h"
#include "FrameWork/Debugger/DebugFps.h"
#include "FrameWork/Sound/SoundManager.h"
#include "GameSystem/Collision/CollisionManager.h"

#include "FrameWork/Rendering/DefferedSprite.h"
#include "FrameWork/Rendering/BlurSprite.h"
#include "FrameWork/Scene/SceneManager.h"

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CGameSystem::CGameSystem()
: m_DefferdSprite(nullptr)
, m_GetBrightSprite(nullptr)
, m_BlurBrightSprite(nullptr)
, m_PostEffectSprite(nullptr)
#if defined(_DEBUG)
, m_DebugFps( nullptr )
#endif
{
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CGameSystem::~CGameSystem()
{
	Term();
}

//---------------------------------------------------------------------------
/*!
	@brief	破棄
*/
//---------------------------------------------------------------------------
void CGameSystem::Term()
{
	SAFE_DELETE_TASK(m_DefferdSprite);
	SAFE_DELETE_TASK(m_BlurBrightSprite);
	SAFE_DELETE_TASK(m_GetBrightSprite);
	SAFE_DELETE_TASK(m_PostEffectSprite);
#if defined(_DEBUG)
	SAFE_DELETE(m_DebugFps);
#endif
	// マネージャー系破棄
	CWindow::DestroyInstance();
	DXMng::DestroyInstance();

	CSoundManager::DestroyInstance();
	CSceneManager::DestroyInstance();
	CCollisionManager::DestroyInstance();
#if defined(_DEBUG)
	CDebugProfile::DestroyInstance();
#endif
	CFontManager::DestroyInstance();
	CInputManager::DestroyInstance();
	CCameraManager::DestroyInstance();
	CRenderingManager::DestroyInstance();
	CShaderManager::DestroyInstance();
	CLoaderManager::DestroyInstance();
	CAssetManager::DestroyInstance();
	CTaskScheduler::DestroyInstance();
}

//---------------------------------------------------------------------------
/*!
	@brief	破棄
*/
//---------------------------------------------------------------------------
void CGameSystem::Destory()
{
	Term();
}

//---------------------------------------------------------------------------
/*!
	@brief	初期化
*/
//---------------------------------------------------------------------------
bool CGameSystem::Init()
{
	//Win32ウィンドウの作成、DirectXの初期化
	CWindow::CreateInstance();

	CWindow* cpWindow = CWindow::GetInstance();
	cpWindow->CreateGameWindow(WINDOW_WIDTH, WINDOW_HEIGHT, _T("ntg"));

	DXMng::CreateInstance();
	DXMng* pcDX = DXMng::GetInstance();
	pcDX->InitDirectX(cpWindow->m_Hwnd, DXMng::eWINDOWMODE_SCREEN);

	//--------------------------------------------------------------------
	// マネージャ作成
	CTaskScheduler::CreateInstance();
	CAssetManager::CreateInstance();
	CLoaderManager::CreateInstance();
	CShaderManager::CreateInstance();
	CRenderingManager::CreateInstance();
	CCameraManager::CreateInstance();
	CInputManager::CreateInstance();
	CFontManager::CreateInstance();
#if defined(_DEBUG)
	CDebugProfile::CreateInstance();
#endif
	CCollisionManager::CreateInstance();
	CSceneManager::CreateInstance();
	CSoundManager::CreateInstance();

	//カメラを指定
	CCameraManager* pcCameraM = CCameraManager::GetInstance();
	if (pcCameraM)
	{
		pcCameraM->SetActiveCameraID(CCameraManager::eCAMERAID_MAIN);
	}

#if defined(_DEBUG)
	CLASS_NEW(m_DebugFps, CFPSCounter);
#endif

	CLASS_NEW(m_DefferdSprite, CDefferedSprite());
	if (m_DefferdSprite)
	{
		m_DefferdSprite->SetScale(NSVector3(static_cast<f32>(WINDOW_WIDTH), static_cast<f32>(WINDOW_HEIGHT), 0.0f));
		m_DefferdSprite->SetPos(NSVector3(static_cast<f32>(WINDOW_WIDTH)*0.5f, static_cast<f32>(WINDOW_HEIGHT)*0.5f, 0.0f));

		CRenderingContext cContext;
		cContext.SetShader(CShaderManager::eSHADERID_DEFFERD);
		m_DefferdSprite->SetRenderContext(cContext);
		m_DefferdSprite->SetDrawEnable(true, CRenderingManager::E_RENDER_STAGE_MAIN);
	}

	CLASS_NEW(m_GetBrightSprite, CSprite());
	if (m_GetBrightSprite)
	{
		m_GetBrightSprite->SetScale(NSVector3(static_cast<f32>(WINDOW_WIDTH), static_cast<f32>(WINDOW_HEIGHT), 0.0f));
		m_GetBrightSprite->SetPos(NSVector3(static_cast<f32>(WINDOW_WIDTH)*0.5f, static_cast<f32>(WINDOW_HEIGHT)*0.5f, 0.0f));

		CRenderingContext cContext;
		cContext.SetShader(CShaderManager::eSHADERID_GETBRIGHT);

		m_GetBrightSprite->SetRenderContext(cContext);
		m_GetBrightSprite->SetDrawEnable(true, CRenderingManager::E_RENDER_GET_BRIGHT);
	}

	CLASS_NEW(m_BlurBrightSprite, CBlurSprite());
	if (m_BlurBrightSprite)
	{
		m_BlurBrightSprite->SetScale(NSVector3(static_cast<f32>(WINDOW_WIDTH), static_cast<f32>(WINDOW_HEIGHT), 0.0f));
		m_BlurBrightSprite->SetPos(NSVector3(static_cast<f32>(WINDOW_WIDTH)*0.5f, static_cast<f32>(WINDOW_HEIGHT)*0.5f, 0.0f));

		CRenderingContext cContext;
		cContext.SetShader(CShaderManager::eSHADERID_BLUR);

		m_BlurBrightSprite->SetRenderContext(cContext);
		m_BlurBrightSprite->SetDrawEnable(true, CRenderingManager::E_RENDER_BLUR_BRIGHT);
	}

	CLASS_NEW(m_PostEffectSprite, CSprite());
	if (m_PostEffectSprite)
	{
		m_PostEffectSprite->SetScale(NSVector3(static_cast<f32>(WINDOW_WIDTH), static_cast<f32>(WINDOW_HEIGHT), 0.0f));
		m_PostEffectSprite->SetPos(NSVector3(static_cast<f32>(WINDOW_WIDTH)*0.5f, static_cast<f32>(WINDOW_HEIGHT)*0.5f, 0.0f));

		CRenderingContext cContext;
		cContext.SetShader(CShaderManager::eSHADERID_POSTEFFECT);

		m_PostEffectSprite->SetRenderContext(cContext);
		m_PostEffectSprite->SetDrawEnable(true, CRenderingManager::E_RENDER_STAGE_POSTEFFECT);
	}

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	Waiting
*/
//---------------------------------------------------------------------------
void Wait(DWORD wait_time)
{
    MSG msg;
    DWORD start_time = timeGetTime();

    do
	{
        // メッセージ処理
        if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
		{
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }

        if( wait_time > 0 ) 
		{
			Sleep( 1 ); // ちょっと休憩（CPUの占有率を下げるため）
		}
    }
	while( timeGetTime() < wait_time + start_time ); // wait_time だけ回る
}

//---------------------------------------------------------------------------
/*!
	@brief	ゲームループ
*/
//---------------------------------------------------------------------------
bool CGameSystem::GameLoop()
{
	MSG msg;
	SecureZeroMemory( &msg, sizeof( msg ));
	bool bQuit = false;

	CWindow* const pcWindow = CWindow::GetInstance();

    DWORD StartTime, EndTime, PassTime;

	while( !bQuit )
	{
		if( !bQuit )
		{
			StartTime = timeGetTime();

			//ゲーム処理
			GameProc();

			//描画
			GameDraw();

			//デバイスチェック
			if( !CheckDevice() || (pcWindow != nullptr && pcWindow->m_bWindowEnd))
			//if( !CheckDevice() )
			{
				bQuit = true;
			}

			//FPS調整
			EndTime = timeGetTime();
			PassTime = EndTime - StartTime; // 経過時間の計算
			(1000 / FPS > PassTime) ? Wait(1000 / FPS - PassTime) : Wait(0); // 待つ。
		}
	}

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	ゲーム更新
*/
//---------------------------------------------------------------------------
bool CGameSystem::GameProc()
{		
//	//--------------------------------------------------------------------
//	// 更新の一番最初で呼ぶ必要があります。
//	//--------------------------------------------------------------------
//	CDebugProfile* pcDebugProfile = CDebugProfile::GetInstance();
//	if( pcDebugProfile )
//	{
//		pcDebugProfile->Update();
//	}

	CInputManager* pcInputM = CInputManager::GetInstance();
	NS_ASSERT(pcInputM);
#if defined( _DEBUG )
	//--------------------------------------------------------------------
	// デバッグカメラ切り替え
	//--------------------------------------------------------------------
	if (pcInputM->IsTrigger(eKEYBOARD_LCTRL, eINPUT_DEVICE_TYPE_KEYBOARD))
	{
		if (CCameraManager::GetInstance()->GetActiveCameraID() == CCameraManager::eCAMERAID_DEBUG)
		{
			CCameraManager::GetInstance()->SetActiveCameraID(CCameraManager::eCAMERAID_MAIN);
		}
		else
		{
			CCameraManager::GetInstance()->SetActiveCameraID(CCameraManager::eCAMERAID_DEBUG);
		}
	}
#endif

//
//	if( pcDebugProfile )
//	{
//		pcDebugProfile->StartProfile( nullptr, _T( "update" ));
//		pcDebugProfile->StartProfile( nullptr, _T("input"));
//	}
//
	//--------------------------------------------------------------------
	// 入力受付
	//--------------------------------------------------------------------
	pcInputM->Update();

	//各種マネージャ系の更新
	CFontManager* pcFontM = CFontManager::GetInstance();
	if (pcFontM != nullptr)
	{
		pcFontM->Update();
	}

	CSoundManager* pcSoundM = CSoundManager::GetInstance();
	NS_ASSERT(pcSoundM);
	pcSoundM->Update(0.0f);

//	if( pcDebugProfile )
//	{
//		pcDebugProfile->EndProfile( nullptr, _T( "input") );
//	}

	CTaskScheduler* pcTaskM = CTaskScheduler::GetInstance();
	if( pcTaskM )
	{
		//pcTaskM->OnPause( CTask::e_Task_Lock_06 );
		s32 nFrameSkip = 1;

#if defined(_DEBUG)
		//フレームスキップ、3倍速
		if (pcInputM->IsPush(eKEYBOARD_LSHIFT, eINPUT_DEVICE_TYPE_KEYBOARD))
		{
			nFrameSkip = 3;
		}
#endif


		for( s32 i = 0; i < nFrameSkip; i++ )
		{
			//タスク回す
			pcTaskM->Update();
		}
	}

	CCollisionManager* pcColM = CCollisionManager::GetInstance();
	NS_ASSERT(pcColM);
	pcColM->CheckAllCollision();

#if defined(_DEBUG)
	if (m_DebugFps)
	{
		double fps = m_DebugFps->GetFPS();
		if (pcFontM != nullptr)
		{
			TCHAR str[64] = { 0 };
			swprintf(str, _T("%d"), (int)fps);
			pcFontM->DrawText2D(str, NSVector2(WINDOW_WIDTH*0.9f, 30.0f), eFONTDATA_FILE_Ms, 0.3f);
		}
	}
#endif

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	ゲーム描画
*/
//---------------------------------------------------------------------------
bool CGameSystem::GameDraw()
{
	//描画処理の発行
	CRenderingManager* pcRenderM = CRenderingManager::GetInstance();
	if (pcRenderM)
	{
		pcRenderM->AllRenderBatch();
	}
	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	デバイスチェック
*/
//---------------------------------------------------------------------------
bool CGameSystem::CheckDevice()
{
	DXMng* pcDXManager = DXMng::GetInstance();
	if( pcDXManager )
	{
		HRESULT hr = pcDXManager->GetDevice().GetDeviceRemovedReason();
		switch( hr )
		{
			case S_OK:
				break;

			case DXGI_ERROR_DEVICE_HUNG:
			case DXGI_ERROR_DEVICE_REMOVED:
			case DXGI_ERROR_DEVICE_RESET:
			case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
			case DXGI_ERROR_INVALID_CALL:
				return false;
		}
	}

	return true;
}


//EOF
