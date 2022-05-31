/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		InputManager.cpp
	@brief		インプットマネージャー
	@author		shiba
	@date		2012/10/13 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "InputManager.h"

#include "InputDeviceMouse.h"
#include "InputDeviceKeyBoard.h"
#include "InputDeviceKeyPad.h"
#include "XInput.h"

#include "FrameWork/Win32/Window.h"

NS_SINGLETON_INSTANCE_DEFINE( CInputManager );

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "xinput.lib")

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CInputManager::CInputManager() 
: m_pDirectInput(nullptr)
, m_pMouse(nullptr)
, m_pKeyboard(nullptr)
{
	m_paPad.clear();
	m_aDiDevice.clear();
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CInputManager::~CInputManager()
{
	SAFE_RELEASE( m_pDirectInput );
	SAFE_DELETE( m_pKeyboard );
	SAFE_DELETE( m_pMouse );

	m_aDiDevice.clear();

	for( std::vector< CInputDeviceKeyPad* >::iterator it = m_paPad.begin(); it != m_paPad.end(); ++it )
	{
		SAFE_DELETE( *it );
	}
	m_paPad.clear();
}

//---------------------------------------------------------------------------
/*!
	@brief	初期化
*/
//---------------------------------------------------------------------------
bool CInputManager::Init()
{
	//--------------------------------------------------------------------
	// DirectInputデバイスを作成
	//--------------------------------------------------------------------
	if( FAILED( DirectInput8Create( GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, nullptr) ) )
	{
		NS_Report(_T("DirectInput8Create Error\n"));
		return false;
	}

	//接続されているデバイスを列挙する。
	if ( FAILED( m_pDirectInput->EnumDevices(DI8DEVCLASS_ALL, EnumDevicesCallback, (void*)&m_aDiDevice, DIEDFL_ATTACHEDONLY) ) ) 
	{
		NS_Report(_T("DirectInput8EnumDevices Error\n"));
		return false;
	}

	//デバイス実体を作成
	CreateDevice();

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	更新
*/
//---------------------------------------------------------------------------
void CInputManager::Update()
{
	if( m_pMouse )
	{
		m_pMouse->Update();
	}
	if( m_pKeyboard )
	{
		m_pKeyboard->Update();
	}
	for( std::vector< CInputDeviceKeyPad* >::iterator it = m_paPad.begin(); it != m_paPad.end(); ++it )
	{
		(*it)->Update();
	}
}

//---------------------------------------------------------------------------
/*!
	@brief		入力されているか？
	@param[in]	unButton	ボタン定義
	@param[in]	eType	入力形式
	@param[in]	nIdx	インデックス。パッドなら1～4
*/
//---------------------------------------------------------------------------
bool CInputManager::IsPush( const u32 unButton, const E_INPUT_DEVICE_TYPE eType, const s32 nIdx /* = 0 */ )
{
CInputDeviceInterface* pcInterface = GetTypeToDevice(eType, nIdx);
if (pcInterface)
{
	return pcInterface->IsPush(unButton);
}
return false;
}

//---------------------------------------------------------------------------
/*!
	@brief		入力された瞬間？
	@param[in]	unButton	ボタン定義
	@param[in]	eType	入力形式
	@param[in]	nIdx	インデックス。パッドなら1～4
*/
//---------------------------------------------------------------------------
bool CInputManager::IsTrigger(const u32 unButton, const E_INPUT_DEVICE_TYPE eType, const s32 nIdx /* = 0 */)
{
	CInputDeviceInterface* pcInterface = GetTypeToDevice(eType, nIdx);
	if (pcInterface)
	{
		return pcInterface->IsTrigger(unButton);
	}
	return false;
}

//---------------------------------------------------------------------------
/*!
	@brief		ボタンを離した瞬間？
	@param[in]	unButton	ボタン定義
	@param[in]	eType	入力形式
	@param[in]	nIdx	インデックス。パッドなら1～4
*/
//---------------------------------------------------------------------------
bool CInputManager::IsRelease(const u32 unButton, const E_INPUT_DEVICE_TYPE eType, const s32 nIdx /* = 0 */)
{
	CInputDeviceInterface* pcInterface = GetTypeToDevice(eType, nIdx);
	if (pcInterface)
	{
		return pcInterface->IsRelease(unButton);
	}
	return false;
}

//---------------------------------------------------------------------------
/*!
	@brief	マウスカーソルの座標取得
*/
//---------------------------------------------------------------------------
const NSVector2& CInputManager::GetMouseCursorPos() const
{
	return m_pMouse->GetMouseCursorPos();
}

//---------------------------------------------------------------------------
/*!
	@brief	マウスの移動ベクトルを獲得
	@return NSVector2 移動ベクトル
*/
//---------------------------------------------------------------------------
const NSVector2& CInputManager::GetMouseCursorDir() const
{
	return m_pMouse->GetMouseCursorDir();
}

//---------------------------------------------------------------------------
/*!
	@brief	マウスの移動ベクトル(正規化)を獲得
	@return NSVector2 正規化向き
*/
//---------------------------------------------------------------------------
const NSVector2 CInputManager::GetMouseCursorDirNom() const
{
	return m_pMouse->GetMouseCursorDirNom();

}

//---------------------------------------------------------------------------
/*!
	@brief	マウスホイール値
*/
//---------------------------------------------------------------------------
const long CInputManager::GetMouseWheelMove() const
{
	if (m_pMouse == nullptr)
	{
		return 0;
	}

	return m_pMouse->GetMouseWheelMove();
}

//---------------------------------------------------------------------------
/*!
	@brief	パッドスティック
*/
//---------------------------------------------------------------------------
const NSVector2 CInputManager::GetLeftStickDir(const int nPadID /* = 0 */) const
{
	NSVector2 vec;
	vec.SetZero();

	if (nPadID  < 0 && nPadID >= CONTROLLER_MAX)
	{
		return vec;
	}

	if (m_paPad[nPadID] == nullptr)
	{
		return vec;
	}

	return m_paPad[nPadID]->GetLeftStickDir();
}

const NSVector2 CInputManager::GetRightStickDir(const int nPadID /* = 0 */) const
{
	NSVector2 vec;
	vec.SetZero();

	if (nPadID < 0 && nPadID >= CONTROLLER_MAX)
	{
		return vec;
	}

	if (m_paPad[nPadID] == nullptr)
	{
		return vec;
	}

	return m_paPad[nPadID]->GetRightStickDir();
}

//---------------------------------------------------------------------------
/*!
	@brief			マウスデバイスの初期化
	@param[in]		const DIDEVICEINSTANCE*		(deviceInstance)	デバイスインスタンス
	@retval			bool						成功／失敗
*/
//---------------------------------------------------------------------------
bool CInputManager::InitMouse( const DIDEVICEINSTANCE* deviceInstance )
{
	if( m_pMouse )
	{
		NS_Report(_T("Already Create Mouse\n"));
		return false;
	}

	// マウスデバイスの作成
	CLASS_NEW(m_pMouse, CInputDeviceMouse() );
	m_pMouse->SetupDevice( m_pDirectInput, deviceInstance );

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief			キーボードデバイスの初期化
	@param[in]		const DIDEVICEINSTANCE*		(deviceInstance)	デバイスインスタンス
	@retval			bool						成功／失敗
*/
//---------------------------------------------------------------------------
bool CInputManager::InitKeyboard( const DIDEVICEINSTANCE* deviceInstance )
{
	if( m_pKeyboard )
	{
		NS_Report(_T("Already Create Keyboard\n"));
		return false;
	}

	// キーボードの作成
	CLASS_NEW(m_pKeyboard, CInputDeviceKeyBoard() );
	m_pKeyboard->SetupDevice( m_pDirectInput, deviceInstance );

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief			パッドデバイスの初期化(XInputを用いる)
	@retval			bool						成功／失敗
*/
//---------------------------------------------------------------------------
bool CInputManager::InitPad()
{
	m_paPad.clear();
	m_paPad.reserve(CONTROLLER_MAX);

	//XInputを使用する
	for (s32 i = 0; i < CONTROLLER_MAX; ++i)
	{
		CInputDeviceKeyPad* pcPad = nullptr;
		CLASS_NEW(pcPad, CInputDeviceKeyPad());

		pcPad->SetPadIndex(i);
		m_paPad.push_back(pcPad);
	}


	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief			入力デバイスの列挙
	@param[in]		const DIDEVICEINSTANCE*		(deviceInstance)	デバイスインスタンス
	@param[in]		void*						(context)			ユーザー引数
	@retval			bool						成功／失敗
*/
//---------------------------------------------------------------------------
BOOL WINAPI CInputManager::EnumDevicesCallback( const LPCDIDEVICEINSTANCE deviceInstance, void* context )
{
	std::vector< DIDEVICEINSTANCE >* ptr = NS_REINTERPRET_CAST( std::vector< DIDEVICEINSTANCE >*, context );
	DIDEVICEINSTANCE tmp = *deviceInstance;
	ptr->push_back( tmp );

#if defined( _DEBUG )

	switch( LOBYTE( tmp.dwDevType ) ) 
	{
		case DI8DEVTYPE_MOUSE:
			NS_Report( _T("Found Device Mouse\n" ));
			break;
		case DI8DEVTYPE_KEYBOARD:
			NS_Report( _T("Found Device Keyboard\n"));
			break;
		//case DI8DEVTYPE_JOYSTICK:
		//case DI8DEVTYPE_GAMEPAD:
		//case DI8DEVTYPE_1STPERSON:
		//	NS_Report( "Device GamePad\n" );
		//	break;
		default:
			break;
	}

#endif

	return DIENUM_CONTINUE;
}

//---------------------------------------------------------------------------
/*!
	@brief	列挙取得されたデバイス
*/
//---------------------------------------------------------------------------
void CInputManager::CreateDevice()
{
	for ( u32 i = 0; i < m_aDiDevice.size(); i++ ) 
	{
		switch( LOBYTE( m_aDiDevice[i].dwDevType ) ) 
		{
			case DI8DEVTYPE_MOUSE:
				InitMouse( &m_aDiDevice[i] );
				break;
			case DI8DEVTYPE_KEYBOARD:
				InitKeyboard( &m_aDiDevice[i] );
				break;

			//case DI8DEVTYPE_JOYSTICK:
			//case DI8DEVTYPE_GAMEPAD:
			//case DI8DEVTYPE_1STPERSON:
			//	InitPad( &m_aDiDevice[i] );
			//	break;

			default:
				break;
		}
	}
	InitPad();
}

//---------------------------------------------------------------------------
/*!
	@brief	タイプからデバイスを返す
*/
//---------------------------------------------------------------------------
CInputDeviceInterface* CInputManager::GetTypeToDevice( const E_INPUT_DEVICE_TYPE eType, const s32 nIdx) const
{
	switch( eType )
	{
		case eINPUT_DEVICE_TYPE_MOUSE:
			return m_pMouse;
		case eINPUT_DEVICE_TYPE_KEYBOARD:
			return m_pKeyboard;
		case eINPUT_DEVICE_TYPE_PAD:
			if (nIdx >= 0 && nIdx < CONTROLLER_MAX)
			{
				return m_paPad[nIdx];
			}
		default:
			break;
	}
	return nullptr;
}

//EOF
