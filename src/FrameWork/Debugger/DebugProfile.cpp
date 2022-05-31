/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		DebugProfile.cpp
	@brief		プロファイラ
	@author		shiba
	@date		2013/1/25 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#if defined( _DEBUG )

#include "DebugProfile.h"

#include "FrameWork/Font/FontManager.h"
#include "FrameWork/Utility/UtilHash.h"

NS_SINGLETON_INSTANCE_DEFINE( CDebugProfile );

//---------------------------------------------------------------------------
/*!
	@brief	コンストラクタ
*/
//---------------------------------------------------------------------------
CDebugProfile::CDebugProfile()
: m_bEnableProfile( false )
{
}

//---------------------------------------------------------------------------
/*!
	@brief	デストラクタ
*/
//---------------------------------------------------------------------------
CDebugProfile::~CDebugProfile()
{
	m_Profile.clear();
}

//---------------------------------------------------------------------------
/*!
	@brief	初期化
*/
//---------------------------------------------------------------------------
bool CDebugProfile::Init()
{
	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	更新(呼び出しタイミングをしっかりしないといけないのよね･･･)
*/
//---------------------------------------------------------------------------
bool CDebugProfile::Update()
{
	std::vector< CProfileOne >::iterator it = m_Profile.begin();
	for( ; it != m_Profile.end(); ++it )
	{
		(*it).m_tStartTime = 0;
		(*it).m_tTime = 0;
		(*it).unCount = 0;
	}

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	プロファイル開始
*/
//---------------------------------------------------------------------------
bool CDebugProfile::StartProfile( const TCHAR* strParentProfileName, const TCHAR* strProfileName )
{
	NS_UNUSED( strParentProfileName );

	u32 strHashID = CUtilHash::calcNameHash( strProfileName );

	//現在のクロックタイム取得
	LARGE_INTEGER time;
	QueryPerformanceCounter( &time );

	//デバッグメニューを調整
	if( m_Profile.size() > 0 )
	{
		std::vector< CProfileOne >::iterator it = m_Profile.begin();
		for( ; it != m_Profile.end(); ++it )
		{
			if( (*it).strHashID == strHashID )
			{
				//存在してる。差分の開始時間を取得
				(*it).m_tStartTime = time.QuadPart;
				(*it).unCount++;

				return true;
			}
		}

	}

	//存在しないので、プロファイル項目追加
	CProfileOne sProfile;
	sProfile.strHashID = strHashID;
	sProfile.m_tStartTime = time.QuadPart;
	sProfile.unCount = 1;
	sProfile.strProfileName = strProfileName;
	m_Profile.push_back( sProfile );

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	プロファイル終了
*/
//---------------------------------------------------------------------------
bool CDebugProfile::EndProfile( const TCHAR* strParentProfileName, const TCHAR* strProfileName )
{
	NS_UNUSED( strParentProfileName );

	//デバッグメニューを調整
	u32 strHashID = CUtilHash::calcNameHash( strProfileName );
	std::vector< CProfileOne >::iterator it = m_Profile.begin();
	for( ; it != m_Profile.end(); ++it )
	{
		if( (*it).strHashID == strHashID ) 
		{
			LARGE_INTEGER time;
			QueryPerformanceCounter( &time );
			time_t endTime = time.QuadPart;
			time_t elpasedTime = endTime - (*it).m_tStartTime;
			//足しこむ
			(*it).m_tTime += elpasedTime;

			return true;
		}
	}

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	プロファイルを画面更新
*/
//---------------------------------------------------------------------------
bool CDebugProfile::FlushProfile()
{
	if( m_bEnableProfile )
	{
		CFontManager* pcFontM = CFontManager::GetInstance();
		NS_ASSERT( pcFontM );

		std::vector< CProfileOne >::iterator it = m_Profile.begin();
		for( s32 i = 0; it != m_Profile.end(); ++it )
		{
			LARGE_INTEGER nFreq;
			QueryPerformanceFrequency( &nFreq );  // クロック周波数

			//時間
			//double deltaTime = static_cast<double>( (*it).m_tTime ) * 1000.0f;
			//double dTime = deltaTime / static_cast<double>( nFreq.QuadPart);

			TCHAR str[ 64 ] = { 0 };
			//sprintf_s( str, sizeof( str ), "%s:%.2fms:C:%d", (*it).strProfileName, dTime, (*it).unCount );

			NSVector2 vPos;
			vPos.x = 50.0f;
			vPos.y = ( i + 1 ) * 50.0f;

			//pcFontM->DrawText2D( str, vPos, eFONTDATA_FILE_Azu );
			i++;
		}
	}

	return true;
}

//---------------------------------------------------------------------------
/*!
	@brief	CSVでプロファイルデータ書き出し
*/
//---------------------------------------------------------------------------
bool CDebugProfile::SaveCSV()
{
	return true;
}

#endif // defined( _DEBUG )

//EOF
