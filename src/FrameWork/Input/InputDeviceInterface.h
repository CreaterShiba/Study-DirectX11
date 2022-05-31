
#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		InputDeviceInterface.h
	@brief		インプットデバイスのベースクラス
	@author		shiba
	@date		2013/1/13 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/
#include "Framework/Utility/UtilDefine.h"
#include "FrameWork/Input/InputDefine.h"

/*==========================================================================*/
/*!
	@class	CInputDeviceInterface
	@brief	インプットデバイスインターフェイス
*/
/*==========================================================================*/
class CInputDeviceInterface
{
	public:
		CInputDeviceInterface() :
		m_eInputDeviceType( eINPUT_DEVICE_TYPE_INV ),
		m_bEnable( true )
		{};
		virtual ~CInputDeviceInterface(){};

		//@brief 初期化
		virtual bool Init() = 0;
		//@brief 更新
		virtual bool Update() = 0;

		//@brief 押されている？
		virtual	bool IsPush( const u32 unButton ) = 0;
		//@brief 押された瞬間である？
		virtual	bool IsTrigger( const u32 unButton ) = 0;
		//@brief 離された瞬間である？
		virtual	bool IsRelease( const u32 unButton ) = 0;						
		//@brief 連打されている？
		//virtual	bool IsRepeat( const u32 unButton ) = 0;

		//@brief 有効無効(タスク継承したらいらなくなりますよ。)設定
		bool IsInputDeviceEnable() const
		{
			return m_bEnable;
		}
		//@brief 有効無効返す
		void SetInputDeviceEnable( const bool bEnable )
		{
			m_bEnable = bEnable;
		}
			
	protected:
		E_INPUT_DEVICE_TYPE m_eInputDeviceType;	//!< デバイスタイプ

	private:
		bool m_bEnable;	//!< インプット有効、無効

};

//EOF