/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		InputDeviceKeyBoard.cpp
	@brief		キーボード入力
	@author		shiba
	@date		2013/1/13 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "InputDeviceKeyBoard.h"
#include "FrameWork/Win32/Window.h"

// DirectInputのキーボードのキーコードマップ
unsigned char CInputDeviceKeyBoard::m_KeyboardMap[KEYBOARD_KEYMAX] =
{
	DIK_A,			// A,
	DIK_B,			// B,
	DIK_C,			// C,
	DIK_D,			// D,
	DIK_E,			// E,
	DIK_F,			// F,
	DIK_G,			// G,
	DIK_H,			// H,
	DIK_I,			// I,
	DIK_J,			// J,
	DIK_K,			// K,
	DIK_L,			// L,
	DIK_M,			// M,
	DIK_N,			// N,
	DIK_O,			// O,
	DIK_P,			// P,
	DIK_Q,			// Q,
	DIK_R,			// R,
	DIK_S,			// S,
	DIK_T,			// T,
	DIK_U,			// U,
	DIK_V,			// V,
	DIK_W,			// W,
	DIK_X,			// X,
	DIK_Y,			// Y,
	DIK_Z,			// Z,
	DIK_1,			// 1,
	DIK_2,			// 2,
	DIK_3,			// 3,
	DIK_4,			// 4,
	DIK_5,			// 5,
	DIK_6,			// 6,
	DIK_7,			// 7,
	DIK_8,			// 8,
	DIK_9,			// 9,
	DIK_0,			// 0,
	DIK_ESCAPE,		// Escape,
	DIK_F1,			// F1,
	DIK_F2,			// F2,
	DIK_F3,			// F3,
	DIK_F4,			// F4,
	DIK_F5,			// F5,
	DIK_F6,			// F6,
	DIK_F7,			// F7,
	DIK_F8,			// F8,
	DIK_F9,			// F9,
	DIK_F10,		// F10,
	DIK_F11,		// F11,
	DIK_F12,		// F12,
	DIK_RETURN,		// Enter,
	DIK_BACK,		// Backspace,
	DIK_TAB,		// Tab,
	DIK_SPACE,		// Space,
	DIK_MINUS,		// Minus,
	DIK_EQUALS,		// Equals,
	DIK_LBRACKET,	// LeftBracket,
	DIK_RBRACKET,	// RightBracket,
	DIK_BACKSLASH,	// Backslash,
	DIK_SEMICOLON,	// Semicolon
	DIK_APOSTROPHE,	// Apostrophe
	DIK_COMMA,		// Comma,
	DIK_PERIOD,		// Period,
	DIK_SLASH,		// Slash,
	DIK_LEFT,		// Left,
	DIK_RIGHT,		// Right,
	DIK_UP,			// Up,
	DIK_DOWN,		// Down,
	DIK_HOME,		// Home,
	DIK_END,		// End,
	DIK_PRIOR,		// PageUp,
	DIK_NEXT,		// PageDown,
	DIK_INSERT,		// Insert,
	DIK_DELETE,		// Delete,
	DIK_NUMPAD1,	// Numpad_1,
	DIK_NUMPAD2,	// Numpad_2,
	DIK_NUMPAD3,	// Numpad_3,
	DIK_NUMPAD4,	// Numpad_4,
	DIK_NUMPAD5,	// Numpad_5,
	DIK_NUMPAD6,	// Numpad_6,
	DIK_NUMPAD7,	// Numpad_7,
	DIK_NUMPAD8,	// Numpad_8,
	DIK_NUMPAD9,	// Numpad_9,
	DIK_NUMPAD0,	// Numpad_0,
	DIK_DIVIDE,		// Numpad_Slash,
	DIK_MULTIPLY,	// Numpad_Asterisk,
	DIK_SUBTRACT,	// Numpad_Minus,
	DIK_ADD,		// Numpad_Plus,
	DIK_NUMPADENTER,// Numpad_Enter,
	DIK_DECIMAL,	// Numpad_Period,
	DIK_LCONTROL,	// L Ctrl
	DIK_RCONTROL,	// R Ctrl
	DIK_LSHIFT,		// L Shift
	DIK_RSHIFT,		// R Shift
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,
};

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CInputDeviceKeyBoard::CInputDeviceKeyBoard()
: m_pDevice(nullptr)
, m_ucTempKeyData()
, m_nFirstRepeatCount( DEFAULT_REPEAT_FIRST_CNT )
, m_nNextRepeatCount( DEFAULT_REPEAT_NEXT_CNT )
{
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CInputDeviceKeyBoard::~CInputDeviceKeyBoard()
{
	m_pDevice->Unacquire();
	SAFE_RELEASE( m_pDevice );
}

//---------------------------------------------------------------------------
/*!
	@brief	初期化
*/
//---------------------------------------------------------------------------
bool CInputDeviceKeyBoard::Init()
{
	m_eInputDeviceType = eINPUT_DEVICE_TYPE_KEYBOARD;
	
	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	デバイスセットアップ
*/
//---------------------------------------------------------------------------
bool CInputDeviceKeyBoard::SetupDevice( const LPDIRECTINPUT8 pInput, const DIDEVICEINSTANCE* pDevIns )
{
	//PCのに接続されているデバイスとの入力インターフェイスを作成
	if ( FAILED(pInput->CreateDevice(pDevIns->guidInstance, &m_pDevice, nullptr) ) )
	{
		return false;
	}
	//戻ってくるデータのフォーマットを決める
	if ( FAILED(m_pDevice->SetDataFormat(&c_dfDIKeyboard)) ) 
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
bool CInputDeviceKeyBoard::Update()
{
	if( IsInputDeviceEnable() )
	{
		if( m_pDevice )
		{
			HRESULT hr;
			hr = m_pDevice->GetDeviceState( sizeof( m_ucTempKeyData ), m_ucTempKeyData );

			if( FAILED( hr ) ) 
			{
				memset( m_ucTempKeyData, 0, sizeof( m_ucTempKeyData ) );

				//ロストしていたら再取得
				hr = m_pDevice->Acquire();
			}
			else
			{
				//--------------------------------------------------------------------
				// キーボードデータ更新
				CKeyboardData* pKeybordData = &m_cKeyBoardData;
				if ( pKeybordData )
				{
					for ( int i = 0; i < eKEYBOARD_MAX; i++ )
					{
						pKeybordData->byOldButton[i] = pKeybordData->byButton[i];
						pKeybordData->byButton[i] = 0;
						pKeybordData->byTriggerButton[i] = 0;

						int index = m_KeyboardMap[i];
						if ( m_ucTempKeyData[index] & 0x80 )
						{
							pKeybordData->byButton[i] |= KEYBOARD_KEY_ON;
						}

						pKeybordData->byTriggerButton[i] = (~pKeybordData->byOldButton[i] & pKeybordData->byButton[i]);
						pKeybordData->byReleaseButton[i] = (pKeybordData->byOldButton[i] & ~pKeybordData->byButton[i]);
					}

					// ボタンリピート
					for ( int i = 0; i < eKEYBOARD_MAX; i++ )
					{
						pKeybordData->byRepeatButton[i] = 0;

						int index = m_KeyboardMap[i];
						if ( (m_ucTempKeyData[index] & 0x80) )
						{
							// 押されてる
							pKeybordData->nRepeatCount[i]--;
							if ( pKeybordData->nRepeatCount[i] <= 0 )
							{
								// リピート
								if ( !( pKeybordData->byOldButton[i] & KEYBOARD_KEY_ON ) && m_nFirstRepeatCount > 0 )
								{
									// 初回
									pKeybordData->nRepeatCount[i] = m_nFirstRepeatCount;
								}
								else
								{
									pKeybordData->nRepeatCount[i] = m_nNextRepeatCount;
								}
								pKeybordData->byRepeatButton[i] |= KEYBOARD_KEY_ON;
							}
						}
						else
						{
							// 押されていない
							pKeybordData->nRepeatCount[i] = 0;
						}
					}
				}
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
bool CInputDeviceKeyBoard::IsPush( const u32 unButton )
{
	return (m_cKeyBoardData.byButton[ unButton ] & KEYBOARD_KEY_ON);
}

//---------------------------------------------------------------------------
/*!
	@brief		入力された瞬間？
	@param[in]	unButton	ボタン定義
*/
//---------------------------------------------------------------------------
bool CInputDeviceKeyBoard::IsTrigger( const u32 unButton )
{
	return (m_cKeyBoardData.byTriggerButton[ unButton ] & KEYBOARD_KEY_ON);
}

//---------------------------------------------------------------------------
/*!
	@brief		ボタンを離した瞬間？
	@param[in]	unButton	ボタン定義
*/
//---------------------------------------------------------------------------
bool CInputDeviceKeyBoard::IsRelease( const u32 unButton )
{
	return (m_cKeyBoardData.byReleaseButton[ unButton ] & KEYBOARD_KEY_ON);
}

//---------------------------------------------------------------------------
/*!
	@brief		連打されているか？
	@param[in]	unButton	ボタン定義
*/
//---------------------------------------------------------------------------
bool CInputDeviceKeyBoard::IsRepeat( const u32 unButton )
{
	return (m_cKeyBoardData.byRepeatButton[ unButton ] & KEYBOARD_KEY_ON);
}

//EOF