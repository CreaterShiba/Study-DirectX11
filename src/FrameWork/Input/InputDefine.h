
#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		InputDefine.h
	@brief		入力定義
	@author		shiba
	@date		2013/1/8 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

//@brief デバイスタイプ
enum E_INPUT_DEVICE_TYPE
{
	eINPUT_DEVICE_TYPE_MOUSE,		//!< マウス
	eINPUT_DEVICE_TYPE_KEYBOARD,	//!< キーボード
	eINPUT_DEVICE_TYPE_PAD,			//!< パッド
	eINPUT_DEVICE_TYPE_MAX,			//!< デバイスタイプ最大

	eINPUT_DEVICE_TYPE_INV = -1,		//!< 無効
};

//@brief 入力定義
enum E_KEYBOARD_BUTTON
{
	eKEYBOARD_NONE = -1,	// 存在しないボタン

	//キーボード
	eKEYBOARD_A = 0,		
	eKEYBOARD_B,			
	eKEYBOARD_C,			
	eKEYBOARD_D,			
	eKEYBOARD_E,			
	eKEYBOARD_F,			
	eKEYBOARD_G,			
	eKEYBOARD_H,			
	eKEYBOARD_I,			
	eKEYBOARD_J,			
	eKEYBOARD_K,			
	eKEYBOARD_L,			
	eKEYBOARD_M,			
	eKEYBOARD_N,			
	eKEYBOARD_O,			
	eKEYBOARD_P,			
	eKEYBOARD_Q,			
	eKEYBOARD_R,			
	eKEYBOARD_S,			
	eKEYBOARD_T,			
	eKEYBOARD_U,			
	eKEYBOARD_V,			
	eKEYBOARD_W,			
	eKEYBOARD_X,			
	eKEYBOARD_Y,			
	eKEYBOARD_Z,			
	eKEYBOARD_1,			
	eKEYBOARD_2,			
	eKEYBOARD_3,			
	eKEYBOARD_4,			
	eKEYBOARD_5,			
	eKEYBOARD_6,			
	eKEYBOARD_7,			
	eKEYBOARD_8,			
	eKEYBOARD_9,			
	eKEYBOARD_0,			
	eKEYBOARD_ESC,			
	eKEYBOARD_F1,			
	eKEYBOARD_F2,			
	eKEYBOARD_F3,			
	eKEYBOARD_F4,			
	eKEYBOARD_F5,			
	eKEYBOARD_F6,			
	eKEYBOARD_F7,			
	eKEYBOARD_F8,			
	eKEYBOARD_F9,			
	eKEYBOARD_F10,			
	eKEYBOARD_F11,			
	eKEYBOARD_F12,			
	eKEYBOARD_RETURN,		
	eKEYBOARD_BACK,			
	eKEYBOARD_TAB,			
	eKEYBOARD_SPACE,		
	eKEYBOARD_MINUS,		
	eKEYBOARD_EQUALS,		
	eKEYBOARD_LBRACKET,		
	eKEYBOARD_RBRACKET,		
	eKEYBOARD_BACKSLASH,	
	eKEYBOARD_SEMICOLON,	
	eKEYBOARD_APOSTROPHE,	
	eKEYBOARD_COMMA,		
	eKEYBOARD_PERIOD,		
	eKEYBOARD_SLASH,		
	eKEYBOARD_LEFT,			
	eKEYBOARD_RIGHT,		
	eKEYBOARD_UP,			
	eKEYBOARD_DOWN,			
	eKEYBOARD_HOME,			
	eKEYBOARD_END,			
	eKEYBOARD_PRIOR,		
	eKEYBOARD_NEXT,			
	eKEYBOARD_INSERT,		
	eKEYBOARD_DELETE,		
	eKEYBOARD_NUMPAD1,		
	eKEYBOARD_NUMPAD2,		
	eKEYBOARD_NUMPAD3,		
	eKEYBOARD_NUMPAD4,		
	eKEYBOARD_NUMPAD5,		
	eKEYBOARD_NUMPAD6,		
	eKEYBOARD_NUMPAD7,		
	eKEYBOARD_NUMPAD8,		
	eKEYBOARD_NUMPAD9,		
	eKEYBOARD_NUMPAD0,		
	eKEYBOARD_DIVIDE,		
	eKEYBOARD_MULTIPLY,		
	eKEYBOARD_SUBTRACT,		
	eKEYBOARD_ADD,			
	eKEYBOARD_NUMPADENTER,	
	eKEYBOARD_DECIMAL,		
	eKEYBOARD_LCTRL,		
	eKEYBOARD_RCTRL,		
	eKEYBOARD_LSHIFT,		
	eKEYBOARD_RSHIFT,		
	eKEYBOARD_MAX
};

enum E_MOUSE_BUTTON
{
	eMOUSE_NONE = -1,	// 存在しないボタン

	//マウス
	eMOUSE_LEFT = 0,
	eMOUSE_RIGHT,
	eMOUSE_CENTER,
	eMOUSE_MAX,
};

enum E_PAD_BUTTON
{
	ePAD_TYPE_NONE = -1,		// 存在しないボタン

	//パッド定義
	ePAD_LLEFT	= 0,		
	ePAD_LDOWN,				
	ePAD_LRIGHT,			
	ePAD_LUP,				
	ePAD_RLEFT,				
	ePAD_RDOWN,				
	ePAD_RRIGHT,			
	ePAD_RUP,				
	ePAD_R1,				
	ePAD_L1,				
	ePAD_R2,				
	ePAD_L2,				
	ePAD_R3,				
	ePAD_L3,				
	ePAD_SELECT,			
	ePAD_START,				

	ePAD_STICK_LLEFT,		
	ePAD_STICK_LDOWN,		
	ePAD_STICK_LRIGHT,		
	ePAD_STICK_LUP,			

	ePAD_STICK_RLEFT,		
	ePAD_STICK_RDOWN,		
	ePAD_STICK_RRIGHT,		
	ePAD_STICK_RUP,			

	ePAD_DUMMY_01,			
	ePAD_DUMMY_02,			
	ePAD_DUMMY_03,			
	ePAD_DUMMY_04,			
	ePAD_DUMMY_05,			
	ePAD_DUMMY_06,			
	ePAD_DUMMY_07,			
	ePAD_DUMMY_08,			

	ePAD_MAX,				
};

//@brief 入力データ
struct InputData
{
	u32		_button;									//!< 現在のボタン状態
	u32		_edge;										//!< 押した瞬間
	u32		_release;									//!< 離した瞬間
	u32		_repeat;									//!< リピート
	//float	_analog[ga::peripheral::PAD_ANALOG_MAX];	//!< アナログ
};


// リピートカウントの初期設定値
enum
{
	DEFAULT_REPEAT_NEXT_CNT = 1,
	DEFAULT_REPEAT_FIRST_CNT = FPS/2,	// fpsの半分
};

//EOF
