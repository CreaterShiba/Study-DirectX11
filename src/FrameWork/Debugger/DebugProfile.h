#pragma once

#if defined( _DEBUG )

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		DebugProfile.h
	@brief		プロファイラ
	@author		shiba
	@date		2013/1/25 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

class CDebugProfile
{
	//シングルトン
	NS_SINGLETON( CDebugProfile );
	CDebugProfile();
	~CDebugProfile();

public:
	//@brief 初期化
	bool Init();
	//@brief 更新
	bool Update();

public:
	//--------------------------------------------------------------------
	// インターフェイス
	//--------------------------------------------------------------------
	//@brief プロファイル起動
	inline void EnableDrawProfile();
	inline void DisableDrawProfile();

	//@brief プロファイル開始
	bool StartProfile( const TCHAR* strParentProfileName, const TCHAR* strProfileName );
	//@brief プロファイル終了
	bool EndProfile( const TCHAR* strParentProfileName, const TCHAR* strProfileName );
	//@brief プロファイル画面更新
	bool FlushProfile();
	//@brief CSVデータでプロファイルデータ書き出し
	bool SaveCSV();

private:
	class CProfileOne
	{
	public:
		CProfileOne()
		: strProfileName(  )
		, strParentProfileName(  )
		, strHashID( 0 )
		, m_tStartTime( 0 )
		, m_tTime( 0 )
		, unCount( 0 )
		{
		}
		const TCHAR* strProfileName;			//!< プロファイル項目名
		const TCHAR* strParentProfileName;	//!< 親プロファイル名
		u32 strHashID;						//!< プロファイラ文字列ハッシュ値
		time_t m_tStartTime;				//!< プロファイル開始時間
		time_t m_tTime;						//!< １フレームあたりのプロファイル終了時間
		u32 unCount;						//!< 呼び出し回数(StartProfile回数)
	};

	std::vector< CProfileOne > m_Profile;	//!< プロファイルデータ
	bool m_bEnableProfile;					//!< プロファイル描画On/Off
};

//--------------------------------------------------------------------
// Inline
//--------------------------------------------------------------------
void CDebugProfile::EnableDrawProfile()
{
	m_bEnableProfile = true;
}

void CDebugProfile::DisableDrawProfile()
{
	m_bEnableProfile = false;
}

#endif // defined( _DEBUG )


//EOF