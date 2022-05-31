
#pragma once
/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		InputManager.h
	@brief		インプットマネージャー
	@author		shiba
	@date		2012/10/13 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "InputDefine.h"

#include "FrameWork/Utility/UtilSafeArray.h"

#include <dinput.h>

class CInputDeviceInterface;
class CInputDeviceMouse;
class CInputDeviceKeyBoard;
class CInputDeviceKeyPad;
/*==========================================================================*/
/*!
	@class	CInputManager
	@brief	インプットマネージャー
*/
/*==========================================================================*/
class CInputManager
{
	//シングルトン
	NS_SINGLETON( CInputManager );
	CInputManager();
	~CInputManager();

public:
	//@brief	初期化
	bool Init();	
	//@brief	更新
	void Update();

	bool IsPush( const u32 unButton, const E_INPUT_DEVICE_TYPE eType, const s32 nIdx = 0 );
	bool IsTrigger( const u32 unButton, const E_INPUT_DEVICE_TYPE eType, const s32 nIdx = 0);
	bool IsRelease( const u32 unButton, const E_INPUT_DEVICE_TYPE eType, const s32 nIdx = 0);
	const NSVector2& GetMouseCursorPos() const;
	const NSVector2& GetMouseCursorDir() const;
	const NSVector2 GetMouseCursorDirNom() const;
	const long GetMouseWheelMove() const;

	const NSVector2 GetLeftStickDir(const int nPadID = 0) const;
	const NSVector2 GetRightStickDir(const int nPadID = 0) const;

private:

	//@brief  入力デバイスの列挙コールバック
	static BOOL WINAPI EnumDevicesCallback( const LPCDIDEVICEINSTANCE deviceInstance, void* context );

	//@brief マウス初期化
	bool InitMouse( const DIDEVICEINSTANCE* deviceInstance );	
	//@brief キーボード初期化
	bool InitKeyboard( const DIDEVICEINSTANCE* deviceInstance );
	//@brief パッド初期化
	//bool InitPad( const DIDEVICEINSTANCE* deviceInstance );		
	bool InitPad();		
	//@brief デバイス作成
	void CreateDevice();
	//@brief タイプからデバイスを返す
	CInputDeviceInterface* GetTypeToDevice( const E_INPUT_DEVICE_TYPE eType, const s32 nIdx = 0 ) const;

public:
	const static s32 CONTROLLER_MAX = 4;

private:
	LPDIRECTINPUT8 m_pDirectInput;	//!< DirectInput

	CInputDeviceMouse* m_pMouse;					//!< マウス
	CInputDeviceKeyBoard* m_pKeyboard;				//!< キーボード

	std::vector< CInputDeviceKeyPad* > m_paPad;		//!< コントローラー
	std::vector< DIDEVICEINSTANCE > m_aDiDevice;	//!< デバイスリスト
};

//EOF
