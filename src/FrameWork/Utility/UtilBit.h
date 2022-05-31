#pragma once

/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UtilBit.h
	@brief		ビットフラグ
	@author		shiba
	@date		2012/10/13 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

/*==========================================================================*/
/*!
	@class	CUtilBit
	@brief	ビット配列を使用したフラグ管理
*/
/*==========================================================================*/
class CUtilBit
{
public:

	CUtilBit() :
	m_unBit( 0 ){}
	~CUtilBit(){}

	//@brief 全部ビット立てる
	inline void AllFlagOn();

	//@brief 全部ビットおろす
	inline void AllFlagOff();

	//@brief フラグたってるか
	inline bool IsFlag( const u32 unBit ) const;

	//@brief フラグ設定
	inline void SetFlagAll( const u32 unBit );

	//@brief フラグ獲得
	inline u32 GetFlag() const;

	//@brief ビットを立てる
	inline void FlagOn( const u32 unOnBit );

	//@brief ビットを消す
	inline void FlagOff( const u32 unOffBit );

	//@brief フラグ設定
	inline void SetFlag( const u32 unBit, const bool bFlag );

private:
	u32 m_unBit;	//!< ビット
};

void CUtilBit::AllFlagOn()
{
	m_unBit = 0xFFFFFFFF;
}

void CUtilBit::AllFlagOff()
{
	m_unBit = 0;
}

bool CUtilBit::IsFlag(  const u32 unBit ) const
{
	return 0 != ( m_unBit & unBit );
}

void CUtilBit::SetFlagAll( const u32 unBit )
{
	m_unBit = unBit;
}

u32 CUtilBit::GetFlag() const
{
	return m_unBit;
}

void CUtilBit::FlagOn( const u32 unOnBit )
{
	m_unBit |= unOnBit;
}

void CUtilBit::FlagOff( const u32 unOnBit )
{
	m_unBit &= ~unOnBit;
}

void CUtilBit::SetFlag( const u32 unBit, const bool bFlag )
{
	if( bFlag )
	{
		FlagOn( unBit );
	}
	else
	{
		FlagOff( unBit );
	}
}

//EOF
