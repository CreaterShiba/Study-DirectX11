#pragma once
/*==========================================================================*/
/*==========================================================================*/
/*!
	@file		UtilSafeArray.h
	@brief		バッファオーバーフローが起こらない汎用配列
	@author		shiba
	@date		2012/9/9 create this file.

*/
/*==========================================================================*/
/*==========================================================================*/

#include "Framework/Utility/UtilDefine.h"
#include "FrameWork/Utility/UtilInterface.h"

/*==========================================================================*/
/*!
	@class	CSafeArray
	@brief	バッファオーバーフローが起こらない汎用サイズ固定配列
*/
/*==========================================================================*/
template< class T, u32 nWorkSize  >
class CSafeArray : public NSUnCopyable
{
public:
	// 例外アクセス用
	class Base
	{
	public:

		Base()
		{
			m_nWorkSize = nWorkSize;
			memset(&mArray, 0, sizeof(mArray));
		}

		T	mArray[ nWorkSize ];	// 配列	nWorkSize = template指定のnWorkSize
		u32 m_nWorkSize;			// 配列サイズ
	};
private:
	Base	mBase;					// 配列

public:
	CSafeArray()
	{
	}

	//配列サイズ
	const u32 GetSize() const
	{
		return mBase.m_nWorkSize;
	}

	//値をセット
	void Set( const u32 nPos, const T& nNum )
	{
		if( IsOverCheck( nPos ) ) return;
		mBase.mArray[ nPos ] = nNum;
	}

	//!< 指定値で埋める
	void ALLSet( const T& nNum )
	{
		for( u32 i = 0; i < mBase.m_nWorkSize; i++)
		{
			mBase.mArray[ i ] = nNum;
		}
	}

	//!< 値を獲得
	T& Get( const u32 nPos )
	{
		NS_ASSERT( !IsOverCheck( nPos ) );
		return mBase.mArray[ nPos ];
	}
	//!< 値を獲得
	const T& GetConst( const u32 nPos )
	{
		NS_ASSERT( !IsOverCheck( nPos ) );
		return mBase.mArray[ nPos ];
	}
	//!< 値のポインタを獲得
	const T* GetPtrConst( const u32 nPos )
	{
		if( IsOverCheck( nPos ) )	return nullptr;
		return &mBase.mArray[ nPos ];
	}

	//!< 値のポインタを獲得
	T* GetPtr( const u32 nPos )
	{
		if( IsOverCheck( nPos ) )	return nullptr;
		return &mBase.mArray[ nPos ];
	}

private:
	//変な演算子はきっておく
	//[]オーバーロード
	T& operator[]( const u32 nPos )
	{
		if( IsOverCheck( nPos ) )	return 0;
		return mBase.mArray[ nPos ];
	}
	//!< オーバーチェック
	bool IsOverCheck( const u32 nPos )
	{
		if( mBase.m_nWorkSize <= nPos || nPos < 0 )
		{
			NS_Report(_T("SafeArrayOverFlow!!\n"));
			return true;
		}
		return false;
	}

};

//EOF

