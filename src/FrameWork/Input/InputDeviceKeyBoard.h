
#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		InputDeviceKeyBoard.h
	@brief		キーボード
	@author		shiba
	@date		2013/1/13 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Input/InputDeviceInterface.h"
#include <dinput.h>

/*==========================================================================*/
/*!
	@class	CInputDeviceKeyBoard
	@brief	キーボード入力
*/
/*==========================================================================*/
class CInputDeviceKeyBoard : public CInputDeviceInterface
{
	private:
		enum 
		{
			KEYBOARD_KEYMAX = 256,	//msdnのページからこのサイズは取得
			KEYBOARD_KEY_OFF = 0,
			KEYBOARD_KEY_ON	= 1<<0,
		};

	public:
		CInputDeviceKeyBoard();
		~CInputDeviceKeyBoard();

	public:
		//@brief 初期化
		virtual bool Init();
		//@brief デバイスのセットアップ
		bool SetupDevice( const LPDIRECTINPUT8 pInput, const DIDEVICEINSTANCE* pDevIns );
		//@brief 更新
		virtual bool Update();

		//@brief 押されている？
		virtual	bool IsPush( const u32 unButton );
		//@brief 押された瞬間である？
		virtual	bool IsTrigger( const u32 unButton );
		//@brief 離された瞬間である？
		virtual	bool IsRelease( const u32 unButton );						
		//@brief 連打されている？
		virtual	bool IsRepeat( const u32 unButton );

	public:
		//キーボード入力データ
		class CKeyboardData
		{
		public:
			CKeyboardData()
			: byButton()
			, byOldButton()
			, byTriggerButton()
			, byReleaseButton()
			, byRepeatButton()
			, nRepeatCount()
			{}

			u8 byButton[KEYBOARD_KEYMAX];
			u8 byOldButton[KEYBOARD_KEYMAX];
			u8 byTriggerButton[KEYBOARD_KEYMAX];	//!< ボタンが押された
			u8 byReleaseButton[KEYBOARD_KEYMAX];	//!< ボタンが離された
			u8 byRepeatButton[KEYBOARD_KEYMAX];		//!< ボタンリピート
			int	nRepeatCount[KEYBOARD_KEYMAX];		//!< リピートまでの回数
		};

	private:
		LPDIRECTINPUTDEVICE8 m_pDevice;			//!< デバイス
		u8 m_ucTempKeyData[ KEYBOARD_KEYMAX ];	//!< キー状態
		CKeyboardData m_cKeyBoardData;			//!< データ

		s32	m_nFirstRepeatCount;							//!< 初回リピートカウント
		s32	m_nNextRepeatCount;								//!< 初回以降リピートカウント
		//static u32	m_unKeyConv[ eKEYBOARD_MAX ];			//!< キー変換データ
		static u8	m_KeyboardMap[ KEYBOARD_KEYMAX ];		//!< DirectInputのキーボードのキーコードマップ
		//static s32	m_nPadKeyAssign[ KEYBOARD_KEYMAX ];		//!< キーアサイン
};

//EOF
