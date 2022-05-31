
#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		InputDeviceMouse.h
	@brief		マウス
	@author		shiba
	@date		2013/1/13 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Input/InputDeviceInterface.h"

#include <dinput.h>

/*==========================================================================*/
/*!
	@class	CInputDeviceMouse
	@brief	マウス入力
*/
/*==========================================================================*/
class CInputDeviceMouse : public CInputDeviceInterface
{
	public:
		CInputDeviceMouse();
		~CInputDeviceMouse();

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

		//@brief 現在のカーソル座標
		const NSVector2& GetMouseCursorPos() const; 

		//@brief 移動ベクトル
		const NSVector2& GetMouseCursorDir() const;
		//@brief 移動ベクトル(正規化ずみ)
		const NSVector2 GetMouseCursorDirNom() const;

		//@brief マウスホイール移動値
		const long GetMouseWheelMove() const;
	

	public:
		//マウス入力データ
		class CMouseData
		{
		public:
			CMouseData()
			: ucButton( 0 )
			, ucTriggerButton( 0 )
			, ucReleaseButton( 0 )
			{}

			u8 ucButton;			//!< ボタン
			u8 ucTriggerButton;		//!< ボタンが押された
			u8 ucReleaseButton;		//!< ボタンが離された
			NSVector2 vCursorPos;	//!< カーソル
		};

	private:

		enum
		{
			eMOUSE_BUTTON_MAX = 8,	//!< マウスボタン数
		};

		HWND m_hwnd;						//!< ウィンドウハンドル
		s64 m_fWheel;						//!< ホイール値
		LPDIRECTINPUTDEVICE8 m_pDevice;		//!< デバイス
		bool m_bOverWindow;					//!< ウィンドウからはみ出てるかどうか
		CMouseData m_cMouseData;			//!< マウスデータ

		NSVector2 m_vMouseDir;				//!< マウス移動ベクトル
		NSVector2 m_vTempDir;				//!< 向き保存用
		u32 m_unDirGetCount;				//!< 向き保存用カウンタ
		u32 m_unDirGetCountPause;			//!< 向き保存用カウンタ区切り
		long m_lWheelMove;					//!< ホイール移動値
};

//EOF
