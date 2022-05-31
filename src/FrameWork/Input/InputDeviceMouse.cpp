/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		InputDeviceMouse.cpp
	@brief		マウス
	@author		shiba
	@date		2013/1/13 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "InputDeviceMouse.h"
#include "FrameWork/Win32/Window.h"

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CInputDeviceMouse::CInputDeviceMouse() 
: m_hwnd(nullptr)
, m_fWheel( 0 )
, m_pDevice(nullptr)
, m_bOverWindow( false )
, m_vMouseDir()
, m_vTempDir()
, m_unDirGetCount( 0 )
, m_unDirGetCountPause( 3 )
, m_lWheelMove( 0 )
{
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CInputDeviceMouse::~CInputDeviceMouse()
{
	m_pDevice->Unacquire();
	SAFE_RELEASE( m_pDevice );
	m_hwnd = nullptr;
}

//---------------------------------------------------------------------------
/*!
	@brief	初期化
*/
//---------------------------------------------------------------------------
bool CInputDeviceMouse::Init()
{
	m_eInputDeviceType = eINPUT_DEVICE_TYPE_MOUSE;

	CWindow* pcWindow = CWindow::GetInstance();
	NS_ASSERT( pcWindow );
	m_hwnd = pcWindow->m_Hwnd;

	//CInputManager::GetInstance()->GetInputDevice()->CreateDevice( GUID_SysMouse, )

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	デバイスセットアップ
*/
//---------------------------------------------------------------------------
bool CInputDeviceMouse::SetupDevice( const LPDIRECTINPUT8 pInput,  const DIDEVICEINSTANCE* pDevIns )
{
	if( !pDevIns )
	{
		return false;
	}

	//PCのに接続されているデバイスとの入力インターフェイスを作成
	if ( FAILED( pInput->CreateDevice( pDevIns->guidInstance, &m_pDevice, nullptr)) )
	{
		return false;
	}
	//戻ってくるデータのフォーマットを決める
	if ( FAILED( m_pDevice->SetDataFormat( &c_dfDIMouse2 ) ) ) 
	{
		return false;
	}
	//強調レベル設定
	if ( m_pDevice->SetCooperativeLevel( CWindow::GetInstance()->m_Hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE) == DIERR_UNSUPPORTED ) 
	{
		return false;
	}

	m_pDevice->Acquire();

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	更新
*/
//---------------------------------------------------------------------------
bool CInputDeviceMouse::Update()
{
	if( IsInputDeviceEnable() )
	{
		POINT sPoint;

		if( GetCursorPos( &sPoint ) )
		{
			RECT windowRect;	//ウィンドウ上座標
			RECT clientRect;	//クライアント上座標

			if ( GetWindowRect( m_hwnd, &windowRect) && GetClientRect( m_hwnd, &clientRect) )
			{
				//ウィンドウの幅分を演算
				s32 nX, nY, nXFrame, nYFrame;
				nXFrame = GetSystemMetrics(SM_CXSIZEFRAME);
				nYFrame = GetSystemMetrics(SM_CYSIZEFRAME);
				nX = ( sPoint.x - ( windowRect.left + nXFrame ) );
				nY = ( sPoint.y - ( windowRect.top + nYFrame + GetSystemMetrics( SM_CYCAPTION ) ) );

				m_cMouseData.vCursorPos.x = NS_STATIC_CAST( f32, nX );
				m_cMouseData.vCursorPos.y = NS_STATIC_CAST( f32, nY );

				if((nX >= 0) && (nX < ( clientRect.right - clientRect.left ) ) && 
				   (nY >= 0) && (nY < ( clientRect.bottom - clientRect.top ) ) )
				{				
					//ウィンドウ内にある
					m_bOverWindow = false;
				}
				else
				{
					//ウィンドウ内にない
					m_bOverWindow = true;
				}

				//向き
				m_unDirGetCount++;
				if( m_unDirGetCount % m_unDirGetCountPause == 0 )
				{
					//向き算出
					m_vMouseDir.x = m_vTempDir.x - m_cMouseData.vCursorPos.x;
					m_vMouseDir.y = m_vTempDir.y - m_cMouseData.vCursorPos.y;

					//現在座標保存しとく
					m_vTempDir.x = m_cMouseData.vCursorPos.x;
					m_vTempDir.y = m_cMouseData.vCursorPos.y;

					m_unDirGetCount = 0;
				}
			}
		}

		if( m_pDevice )
		{
			DIMOUSESTATE2 dims;
			memset(&dims, 0, sizeof(DIMOUSESTATE2));

			//ロストしていたら再取得させる。
			if( FAILED (m_pDevice->GetDeviceState( sizeof(DIMOUSESTATE2), &dims ) ) )
			{
				m_pDevice->Acquire();
			}
			else
			{
				// ボタン処理
				u32 unOldButton =  m_cMouseData.ucButton;
				m_cMouseData.ucButton = 0;

				if( !m_bOverWindow )
				{				
					//ウィンドウ内処理
					for ( s32 i = 0; i < eMOUSE_BUTTON_MAX; i++ )
					{
						//rgbButtonsは押下されたら128を返すので,0x80
						if ( (dims.rgbButtons[i] & 0x80) )
						{
							m_cMouseData.ucButton |= ( 1 << i );
						}
					}
				}
				else
				{
					//ウィンドウオーバー時処理
					//ひとまず何も入力がないと考えるか
				}

				m_lWheelMove = dims.lZ;
				m_cMouseData.ucTriggerButton = ~unOldButton & m_cMouseData.ucButton;	//押した瞬間
				m_cMouseData.ucReleaseButton = unOldButton & ~m_cMouseData.ucButton;	//離した瞬間
			}
		}
	}

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief		入力されているか？
	@param[in]	unButton	ボタン定義
*/
//---------------------------------------------------------------------------
bool CInputDeviceMouse::IsPush( const u32 unButton )
{
	return ( ( m_cMouseData.ucButton & ( 1 << unButton ) ) != 0 );
}

//---------------------------------------------------------------------------
/*!
	@brief		入力された瞬間？
	@param[in]	unButton	ボタン定義
*/
//---------------------------------------------------------------------------
bool CInputDeviceMouse::IsTrigger( const u32 unButton )
{
	return ( ( m_cMouseData.ucTriggerButton & ( 1 << unButton ) ) != 0 );
}

//---------------------------------------------------------------------------
/*!
	@brief		ボタンを離した瞬間？
	@param[in]	unButton	ボタン定義
*/
//---------------------------------------------------------------------------
bool CInputDeviceMouse::IsRelease( const u32 unButton )
{
	return ( ( m_cMouseData.ucReleaseButton & ( 1 << unButton ) ) != 0 );
}

//---------------------------------------------------------------------------
/*!
	@brief	マウスカーソルの座標取得
*/
//---------------------------------------------------------------------------
const NSVector2& CInputDeviceMouse::GetMouseCursorPos() const
{
	return m_cMouseData.vCursorPos;
}

//---------------------------------------------------------------------------
/*!
	@brief	移動ベクトル
*/
//---------------------------------------------------------------------------
const NSVector2& CInputDeviceMouse::GetMouseCursorDir() const
{
	return m_vMouseDir;
}

//---------------------------------------------------------------------------
/*!
	@brief	移動ベクトル（正規化)
*/
//---------------------------------------------------------------------------
const NSVector2 CInputDeviceMouse::GetMouseCursorDirNom() const
{
	NSVector2 vNomVec = m_vMouseDir;
	vNomVec.Normalize();
	return vNomVec;
}

//---------------------------------------------------------------------------
/*!
	@brief	マウスホイール値取得
*/
//---------------------------------------------------------------------------
const long CInputDeviceMouse::GetMouseWheelMove() const
{
	return m_lWheelMove;
}

//EOF