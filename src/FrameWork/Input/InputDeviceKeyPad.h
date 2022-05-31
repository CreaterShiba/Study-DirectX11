
#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		InputDeviceKeyPad.cpp
	@brief		パッドクラス
	@author		shiba
	@date		2013/1/13 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "FrameWork/Input/InputDeviceInterface.h"

#if 0
#ifdef USE_DIRECTX_SDK
#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\include\xinput.h>
#pragma comment(lib,"xinput.lib")
#elif (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
#include <XInput.h>
#pragma comment(lib,"xinput.lib")	//Win7だとDLLを要求される
#else
#include <XInput.h>
#pragma comment(lib,"xinput9_1_0.lib")
#endif

#else

#include <XInput.h>
#pragma comment(lib,"xinput9_1_0.lib")

#endif
/*==========================================================================*/
/*!
	@class	CInputDeviceKeyPad
	@brief	パッド入力
*/
/*==========================================================================*/
class CInputDeviceKeyPad : public CInputDeviceInterface
{
public:
	CInputDeviceKeyPad();
	~CInputDeviceKeyPad();

public:
	//@brief 初期化
	virtual bool Init();
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

	const NSVector2& GetLeftStickDir() const;
	const NSVector2& GetRightStickDir() const;

	//@brief コントローラのインデックスセット
	inline void SetPadIndex(const s32 nIdx);

private:
	enum
	{
		ePAD_BUTTONMAX = ePAD_MAX,
		ePAD_KEY_OFF = 0,
		ePAD_KEY_ON = 1 << 0,
	};

	//コントローラの状態
	struct CONTROLLER_STATE
	{
		XINPUT_STATE state;
		bool bConnected;
	};
	CONTROLLER_STATE m_cControllerState;

	//入力データ
	class CPadData
	{
	public:
		CPadData()
			: byButton()
			, byOldButton()
			, byTriggerButton()
			, byReleaseButton()
			, byRepeatButton()
			, nRepeatCount()
		{}

		u8 byButton[ePAD_BUTTONMAX];
		u8 byOldButton[ePAD_BUTTONMAX];
		u8 byTriggerButton[ePAD_BUTTONMAX];	//!< ボタンが押された
		u8 byReleaseButton[ePAD_BUTTONMAX];	//!< ボタンが離された
		u8 byRepeatButton[ePAD_BUTTONMAX];	//!< ボタンリピート
		int	nRepeatCount[ePAD_BUTTONMAX];		//!< リピートまでの回数
	};
	CPadData m_PadData;

	s32 m_nPadIndex;							//!< インデックス
	s32	m_nFirstRepeatCount;					//!< 初回リピートカウント
	s32	m_nNextRepeatCount;						//!< 初回以降リピートカウント
	static u32 m_aPadMap[ePAD_BUTTONMAX];		//!< DirectInputのキーボードのキーコードマップ

	NSVector2 m_vLeftPadDir;
	NSVector2 m_vRightPadDir;
};

void CInputDeviceKeyPad::SetPadIndex(const s32 nIdx)
{
	m_nPadIndex = nIdx;
}

//EOF
