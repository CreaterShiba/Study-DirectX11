#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		DebugConsole.h
	@brief		コンソールデバッグ周り
	@author		shiba
	@date		2012/7/24 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

class CConsoleDebug
{
public:
	enum E_OUTLEVEL
	{
		E_OUTLEVEL_LOW,
		E_OUTLEVEL_MIDDLE,
		E_OUTLEVEL_HIGH,
		E_OUTLEVEL_ABSOLUTE	//絶対に常に出力。
	};
	static E_OUTLEVEL m_eVerbosity;	//!<  Output基準レベル

	//チャンネル用意だけする
#if 0
	enum E_OUTCHANNEL
	{
		E_OUTCHANNEL_SYSTEM = 1 << 0,
		E_OUTCHANNEL_UPDATE = 1 << 1,
		E_OUTCHANNEL_RENDER = 1 << 2,
		E_OUTCHANNEL_CHARA	= 1 << 3,
		E_OUTCHANNEL_AI		= 1 << 4,
		E_OUTCHANNEL_NETWORK= 1 << 5,
		E_OUTCHANNEL_CAMERA	= 1 << 6,
		E_OUTCHANNEL_SHADER	= 1 << 7,
		E_OUTCHANNEL_PHY	= 1 << 8,
		E_OUTCHANNEL_DBG	= 1 << 9,
	};
	u32 m_unOutChannel;

	CConsoleDebug() :
	m_unOutChannel( 0 )
	{}
#endif
#if 0
	union
	{
		u32 m_unOutChannelFlags;
		struct
		{
			u32 System : 1;	//システムチャンネル
			u32 Update : 1; //更新チャンネル
			u32 Render : 1; //描画チャンネル
			u32 Chara  : 1; //キャラクタチャンネル
			u32 AI	   : 1; //AIチャンネル
			u32 NetWork: 1; //ネットワークチャンネル
			u32 Camera : 1; //カメラチャンネル
			u32 Shader : 1; //シェーダチャンネル
			u32 Phy    : 1; //物理エンジンチャンネル 
			u32 Debug  : 1;	//デバッグチャンネル
		} m_OutChannel;
	};
#endif

public:
	//@brief コンソール出力
	static void Output(const TCHAR* pBuff, ... );	
	static void Output_LV( const CConsoleDebug::E_OUTLEVEL eVerbosity, const TCHAR* pBuff, ... );	

	//ログファイル書き出し（切り替えできるようにする）

	//クラッシュレポート作成（切り替えできるようにする）

	//スクリーンショット機能

	//ゲーム内コンソールシステム

};

#define NS_Report CConsoleDebug::Output
#define NS_ReportLv CConsoleDebug::Output_LV

//EOF